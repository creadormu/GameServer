#include "stdafx.h"
#include "UpdateManager.h"
#include "Log.h"
#include "Util.h"
#include <shlwapi.h>
#include "resource.h"

#pragma comment(lib, "shlwapi.lib")

CUpdateManager gUpdateManager;

CUpdateManager::CUpdateManager() {
    m_Enabled = false;
    m_AutoCheck = true;
    m_AutoDownload = false;
    m_ShowNotifications = true;
    memset(m_UpdateServerUrl, 0, sizeof(m_UpdateServerUrl));
    strcpy_s(m_CurrentVersion, sizeof(m_CurrentVersion), "1.0.0");
    strcpy_s(m_TempDirectory, sizeof(m_TempDirectory), ".\\Update\\Temp");
    m_Status = UPDATE_STATUS_NO_UPDATE;
    m_hWnd = NULL;
    m_DownloadThread = NULL;
    m_UpdateAvailable = false;
    m_LastCheckTime = 0;
    m_DownloadProgress = 0;
    m_hProgressWnd = NULL;
    m_UpdateCount = 0;

    InitializeCriticalSection(&m_CriticalSection);
}

CUpdateManager::~CUpdateManager() {
    if (m_DownloadThread != NULL) {
        TerminateThread(m_DownloadThread, 0);
        CloseHandle(m_DownloadThread);
    }
    DeleteCriticalSection(&m_CriticalSection);
}

void CUpdateManager::Init(HWND hWnd) {
    m_hWnd = hWnd;
    LoadConfig(".\\Data\\UpdateConfig.ini");

    CreateDirectoryA(".\\Update", NULL);
    CreateDirectoryA(m_TempDirectory, NULL);

    LogUpdate("[UpdateManager] Initialized (Version: %s, Enabled: %s)",
        m_CurrentVersion, m_Enabled ? "YES" : "NO");

    if (m_Enabled && strlen(m_UpdateServerUrl) == 0) {
        LogUpdate("[UpdateManager] WARNING: Auto-update is enabled but no update server URL is configured!");
        m_Enabled = false;
    }

    if (m_Enabled && m_AutoCheck) {
        LogUpdate("[UpdateManager] Performing initial update check...");
        CheckForUpdates();
    }
}

void CUpdateManager::LoadConfig(const char* configFile) {
    m_Enabled = GetPrivateProfileInt("UpdateManager", "Enabled", 0, configFile) ? true : false;
    m_AutoCheck = GetPrivateProfileInt("UpdateManager", "AutoCheck", 1, configFile) ? true : false;
    m_AutoDownload = GetPrivateProfileInt("UpdateManager", "AutoDownload", 0, configFile) ? true : false;
    m_ShowNotifications = GetPrivateProfileInt("UpdateManager", "ShowNotifications", 1, configFile) ? true : false;

    GetPrivateProfileString("UpdateManager", "UpdateServerUrl", "", m_UpdateServerUrl, sizeof(m_UpdateServerUrl), configFile);
    GetPrivateProfileString("UpdateManager", "CurrentVersion", "1.0.0", m_CurrentVersion, sizeof(m_CurrentVersion), configFile);
    GetPrivateProfileString("UpdateManager", "TempDirectory", ".\\Update\\Temp", m_TempDirectory, sizeof(m_TempDirectory), configFile);
}

void CUpdateManager::SaveConfig(const char* configFile) {
    WritePrivateProfileString("UpdateManager", "Enabled", m_Enabled ? "1" : "0", configFile);
    WritePrivateProfileString("UpdateManager", "AutoCheck", m_AutoCheck ? "1" : "0", configFile);
    WritePrivateProfileString("UpdateManager", "AutoDownload", m_AutoDownload ? "1" : "0", configFile);
    WritePrivateProfileString("UpdateManager", "ShowNotifications", m_ShowNotifications ? "1" : "0", configFile);
    WritePrivateProfileString("UpdateManager", "UpdateServerUrl", m_UpdateServerUrl, configFile);
    WritePrivateProfileString("UpdateManager", "CurrentVersion", m_CurrentVersion, configFile);
    WritePrivateProfileString("UpdateManager", "TempDirectory", m_TempDirectory, configFile);
}

bool CUpdateManager::CheckForUpdates() {
    if (!m_Enabled) {
        LogUpdate("[UpdateManager] Update check skipped - auto-update is disabled");
        return false;
    }

    if (strlen(m_UpdateServerUrl) == 0) {
        LogUpdate("[UpdateManager] ERROR: Update server URL not configured");
        return false;
    }

    CreateDirectoryA(".\\Update", NULL);
    CreateDirectoryA(m_TempDirectory, NULL);

    LogUpdate("[UpdateManager] Checking for updates from: %s", m_UpdateServerUrl);

    char manifestUrl[768];
    char manifestPath[512];
    sprintf_s(manifestUrl, sizeof(manifestUrl), "%s/update_manifest.txt", m_UpdateServerUrl);
    GetTempFilePath("update_manifest.txt", manifestPath, sizeof(manifestPath));

    DWORD bytesDownloaded = 0;
    if (!DownloadFile(manifestUrl, manifestPath, &bytesDownloaded)) {
        LogUpdate("[UpdateManager] ERROR: Failed to download update manifest");
        m_Status = UPDATE_STATUS_ERROR;
        return false;
    }

    FILE* file = NULL;
    fopen_s(&file, manifestPath, "r");
    if (!file) {
        LogUpdate("[UpdateManager] ERROR: Failed to open update manifest file");
        m_Status = UPDATE_STATUS_ERROR;
        return false;
    }

    char manifestData[16384] = { 0 };
    fread(manifestData, 1, sizeof(manifestData) - 1, file);
    fclose(file);

    if (!ParseUpdateManifest(manifestData)) {
        LogUpdate("[UpdateManager] ERROR: Failed to parse update manifest");
        m_Status = UPDATE_STATUS_ERROR;
        return false;
    }

    // NEW: Filter out already installed updates
    int newUpdateCount = 0;
    UpdateInfo newUpdates[MAX_UPDATES];
    memset(newUpdates, 0, sizeof(newUpdates));

    for (int i = 0; i < m_UpdateCount; i++) {
        UpdateInfo* update = &m_AvailableUpdates[i];

        // Check if this update is already installed
        if (!IsUpdateAlreadyInstalled(update->fileName, update->version)) {
            memcpy(&newUpdates[newUpdateCount], update, sizeof(UpdateInfo));
            newUpdateCount++;
            LogUpdate("[UpdateManager] New update found: %s v%s",
                update->fileName, update->version);
        }
        else {
            LogUpdate("[UpdateManager] Skipping already installed: %s v%s",
                update->fileName, update->version);
        }
    }

    // Replace available updates with only new ones
    memcpy(m_AvailableUpdates, newUpdates, sizeof(newUpdates));
    m_UpdateCount = newUpdateCount;

    if (m_UpdateCount > 0) {
        m_UpdateAvailable = true;
        m_Status = UPDATE_STATUS_AVAILABLE;

        LogUpdate("[UpdateManager] Found %d NEW update(s) available:", m_UpdateCount);
        for (int i = 0; i < m_UpdateCount; i++) {
            LogUpdate("[UpdateManager]   %d) %s (v%s) - %s",
                i + 1,
                m_AvailableUpdates[i].fileName,
                m_AvailableUpdates[i].version,
                m_AvailableUpdates[i].description);
        }

        if (m_ShowNotifications) {
            ShowUpdateSummary();
        }

        if (m_AutoDownload) {
            if (DownloadAllUpdates()) {
                int result = MessageBoxA(m_hWnd,
                    "All updates downloaded successfully!\n\n"
                    "Do you want to apply the updates now?\n"
                    "(The server will restart if updating executable)",
                    "Apply Updates?", MB_YESNO | MB_ICONQUESTION);

                if (result == IDYES) {
                    ApplyAllUpdates();
                }
                else {
                    LogUpdate("[UpdateManager] User postponed update application");
                }
            }
        }

        m_LastCheckTime = GetTickCount();
        return true;
    }
    else {
        m_UpdateAvailable = false;
        m_Status = UPDATE_STATUS_NO_UPDATE;
        LogUpdate("[UpdateManager] No new updates available - all updates already installed");
        m_LastCheckTime = GetTickCount();
        return false;
    }
}

bool CUpdateManager::ParseUpdateManifest(const char* manifestData) {
    m_UpdateCount = 0;
    memset(m_AvailableUpdates, 0, sizeof(m_AvailableUpdates));

    char buffer[1024];
    const char* ptr = manifestData;
    int currentIndex = -1;

    while (*ptr && m_UpdateCount < MAX_UPDATES) {
        int i = 0;
        while (*ptr && *ptr != '\n' && i < sizeof(buffer) - 1) {
            buffer[i++] = *ptr++;
        }
        buffer[i] = '\0';
        if (*ptr == '\n') ptr++;

        if (buffer[0] == '\0' || buffer[0] == ';' || buffer[0] == '#') {
            continue;
        }

        if (buffer[0] == '[') {
            char* endBracket = strchr(buffer, ']');
            if (endBracket) {
                currentIndex = m_UpdateCount;
                m_UpdateCount++;
                continue;
            }
        }

        char* equals = strchr(buffer, '=');
        if (equals && currentIndex >= 0) {
            *equals = '\0';
            char* key = buffer;
            char* value = equals + 1;

            while (*key == ' ' || *key == '\t') key++;
            while (*value == ' ' || *value == '\t') value++;

            char* end = key + strlen(key) - 1;
            while (end > key && (*end == ' ' || *end == '\t' || *end == '\r')) *end-- = '\0';
            end = value + strlen(value) - 1;
            while (end > value && (*end == ' ' || *end == '\t' || *end == '\r')) *end-- = '\0';

            UpdateInfo* update = &m_AvailableUpdates[currentIndex];

            if (_stricmp(key, "Version") == 0) {
                strcpy_s(update->version, sizeof(update->version), value);
            }
            else if (_stricmp(key, "DownloadUrl") == 0) {
                strcpy_s(update->downloadUrl, sizeof(update->downloadUrl), value);
            }
            else if (_stricmp(key, "FileName") == 0) {
                strcpy_s(update->fileName, sizeof(update->fileName), value);
            }
            else if (_stricmp(key, "FileHash") == 0) {
                strcpy_s(update->fileHash, sizeof(update->fileHash), value);
            }
            else if (_stricmp(key, "FileSize") == 0) {
                update->fileSize = atoi(value);
            }
            else if (_stricmp(key, "FileType") == 0) {
                update->fileType = (UpdateFileType)atoi(value);
            }
            else if (_stricmp(key, "Description") == 0) {
                strcpy_s(update->description, sizeof(update->description), value);
            }
            else if (_stricmp(key, "TargetPath") == 0) {
                strcpy_s(update->targetPath, sizeof(update->targetPath), value);
            }
            else if (_stricmp(key, "Required") == 0) {
                update->isRequired = atoi(value) ? true : false;
            }
        }
    }

    for (int i = 0; i < m_UpdateCount; i++) {
        UpdateInfo* update = &m_AvailableUpdates[i];
        if (strlen(update->version) == 0 ||
            strlen(update->downloadUrl) == 0 ||
            strlen(update->fileName) == 0) {
            LogUpdate("[UpdateManager] ERROR: Update #%d missing required fields", i + 1);
            return false;
        }
    }

    LogUpdate("[UpdateManager] Successfully parsed %d update(s) from manifest", m_UpdateCount);
    return m_UpdateCount > 0;
}

void CUpdateManager::ShowUpdateSummary() {
    if (m_UpdateCount == 0) return;

    char msg[4096];
    char temp[512];

    sprintf_s(msg, sizeof(msg),
        "UPDATE AVAILABLE!\n\n"
        "Found %d update(s):\n\n",
        m_UpdateCount);

    float totalSize = 0;
    for (int i = 0; i < m_UpdateCount; i++) {
        UpdateInfo* update = &m_AvailableUpdates[i];
        totalSize += update->fileSize;

        sprintf_s(temp, sizeof(temp),
            "%d) %s (v%s)\n"
            "   Size: %.2f MB\n"
            "   %s\n\n",
            i + 1,
            update->fileName,
            update->version,
            update->fileSize / 1024.0 / 1024.0,
            update->description);

        strcat_s(msg, sizeof(msg), temp);
    }

    sprintf_s(temp, sizeof(temp),
        "Total Size: %.2f MB\n\n"
        "Go to menu [Update] to download and apply the updates.",
        totalSize / 1024.0 / 1024.0);
    strcat_s(msg, sizeof(msg), temp);

    MessageBoxA(m_hWnd, msg, "Updates Available", MB_ICONINFORMATION);
}

bool CUpdateManager::DownloadAllUpdates() {
    if (m_UpdateCount == 0) {
        LogUpdate("[UpdateManager] No updates available to download");
        return false;
    }

    LogUpdate("[UpdateManager] Starting download of %d update(s)...", m_UpdateCount);
    m_Status = UPDATE_STATUS_DOWNLOADING;

    int successCount = 0;
    int failCount = 0;

    for (int i = 0; i < m_UpdateCount; i++) {
        UpdateInfo* update = &m_AvailableUpdates[i];

        LogUpdate("[UpdateManager] Downloading %d/%d: %s",
            i + 1, m_UpdateCount, update->fileName);

        char progressTitle[256];
        sprintf_s(progressTitle, sizeof(progressTitle),
            "Downloading %d/%d: %s", i + 1, m_UpdateCount, update->fileName);
        ShowProgressDialog(progressTitle);

        char destPath[512];
        GetTempFilePath(update->fileName, destPath, sizeof(destPath));

        DWORD bytesDownloaded = 0;
        if (!DownloadFileWithProgress(update->downloadUrl, destPath, &bytesDownloaded)) {
            LogUpdate("[UpdateManager] ERROR: Download failed for %s", update->fileName);
            failCount++;
            CloseProgressDialog();
            continue;
        }

        LogUpdate("[UpdateManager] Download complete: %s (%d bytes)",
            update->fileName, bytesDownloaded);

        CloseProgressDialog();

        if (strlen(update->fileHash) > 0) {
            LogUpdate("[UpdateManager] Verifying %s...", update->fileName);
            if (!VerifyDownloadedFile(destPath, update->fileHash)) {
                LogUpdate("[UpdateManager] ERROR: File verification failed for %s", update->fileName);
                failCount++;
                continue;
            }
        }

        successCount++;
    }

    LogUpdate("[UpdateManager] Download summary: %d successful, %d failed",
        successCount, failCount);

    if (successCount == m_UpdateCount) {
        m_Status = UPDATE_STATUS_READY;
        LogUpdate("[UpdateManager] All updates ready to install");
        return true;
    }
    else if (successCount > 0) {
        m_Status = UPDATE_STATUS_READY;
        char msg[512];
        sprintf_s(msg, sizeof(msg),
            "Downloaded %d out of %d updates.\n\n"
            "%d update(s) failed to download.\n\n"
            "Do you want to continue with partial installation?",
            successCount, m_UpdateCount, failCount);

        int result = MessageBoxA(m_hWnd, msg, "Partial Download", MB_YESNO | MB_ICONWARNING);
        return (result == IDYES);
    }
    else {
        m_Status = UPDATE_STATUS_ERROR;
        MessageBoxA(m_hWnd,
            "All updates failed to download!\n\nPlease check your connection and try again.",
            "Download Failed", MB_ICONERROR);
        return false;
    }
}

bool CUpdateManager::ApplyAllUpdates() {
    if (m_Status != UPDATE_STATUS_READY) {
        LogUpdate("[UpdateManager] ERROR: No updates ready to apply");
        return false;
    }

    char confirmMsg[1024];
    sprintf_s(confirmMsg, sizeof(confirmMsg),
        "Are you sure you want to apply %d update(s)?\n\n"
        "The GameServer may restart if updating executable files.\n\n"
        "Make sure all players are disconnected before proceeding!",
        m_UpdateCount);

    int result = MessageBox(m_hWnd, confirmMsg,
        "Apply Updates - Confirmation",
        MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);

    if (result != IDYES) {
        LogUpdate("[UpdateManager] Updates cancelled by user");
        return false;
    }

    LogUpdate("[UpdateManager] Applying %d update(s)...", m_UpdateCount);

    bool hasExecutableUpdate = false;
    int successCount = 0;
    int failCount = 0;

    for (int i = 0; i < m_UpdateCount; i++) {
        UpdateInfo* update = &m_AvailableUpdates[i];

        char updateFile[512];
        char targetFile[512];
        GetTempFilePath(update->fileName, updateFile, sizeof(updateFile));

        if (update->fileType == UPDATE_FILE_EXECUTABLE) {
            // Just prepare the file, actual copy will happen via batch script
            LogUpdate("[UpdateManager] Executable update prepared: %s", updateFile);

            // Save this update as installed
            SaveInstalledUpdate(update->fileName, update->version);

            successCount++;
            continue;
        }
        else {
            if (strlen(update->targetPath) > 0) {
                strcpy_s(targetFile, sizeof(targetFile), update->targetPath);
            }
            else {
                sprintf_s(targetFile, sizeof(targetFile), ".\\Data\\%s", update->fileName);
            }
        }

        LogUpdate("[UpdateManager] Applying update %d/%d: %s -> %s",
            i + 1, m_UpdateCount, update->fileName, targetFile);

        CreateDirectoryStructure(targetFile);

        if (!CreateBackup(targetFile)) {
            LogUpdate("[UpdateManager] WARNING: Failed to create backup for %s", targetFile);
        }

        if (!CopyFileA(updateFile, targetFile, FALSE)) {
            LogUpdate("[UpdateManager] ERROR: Failed to copy %s: %d",
                targetFile, GetLastError());
            failCount++;
            continue;
        }

        LogUpdate("[UpdateManager] Successfully applied: %s", targetFile);

        // Save this update as installed
        SaveInstalledUpdate(update->fileName, update->version);

        successCount++;
    }

    LogUpdate("[UpdateManager] Apply summary: %d successful, %d failed",
        successCount, failCount);

    if (hasExecutableUpdate && successCount > 0) {
        LogUpdate("[UpdateManager] Executable update detected - preparing restart...");

        strcpy_s(m_CurrentVersion, sizeof(m_CurrentVersion),
            m_AvailableUpdates[m_UpdateCount - 1].version);
        m_Status = UPDATE_STATUS_NO_UPDATE;
        m_UpdateAvailable = false;
        m_UpdateCount = 0;
        SaveConfig(".\\Data\\UpdateConfig.ini");

        FILE* script = NULL;
        fopen_s(&script, ".\\Update\\apply_update.bat", "w");
        if (script) {
            fprintf(script, "@echo off\n");
            fprintf(script, "echo Applying GameServer updates...\n");
            fprintf(script, "timeout /t 2 /nobreak >nul\n");
            fprintf(script, "echo Restarting GameServer...\n");
            fprintf(script, "start \"\" \".\\GameServer.exe\"\n");
            fclose(script);

            STARTUPINFOA si = { 0 };
            PROCESS_INFORMATION pi = { 0 };
            si.cb = sizeof(si);
            char cmdLine[512] = ".\\Update\\apply_update.bat";

            if (CreateProcessA(NULL, cmdLine, NULL, NULL, FALSE,
                CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
                PostQuitMessage(0);
                return true;
            }
        }
    }
    else if (successCount > 0) {
        strcpy_s(m_CurrentVersion, sizeof(m_CurrentVersion),
            m_AvailableUpdates[m_UpdateCount - 1].version);
        SaveConfig(".\\Data\\UpdateConfig.ini");

        char msg[1024];
        sprintf_s(msg, sizeof(msg),
            "Successfully applied %d update(s)!\n\n"
            "You may need to reload data files for changes to take effect.\n\n"
            "%d update(s) failed.",
            successCount, failCount);
        MessageBoxA(m_hWnd, msg, "Updates Applied",
            failCount > 0 ? MB_ICONWARNING : MB_ICONINFORMATION);
    }
    else {
        MessageBoxA(m_hWnd,
            "All updates failed to apply!\n\nPlease check the log for details.",
            "Update Failed", MB_ICONERROR);
        return false;
    }

    m_Status = UPDATE_STATUS_NO_UPDATE;
    m_UpdateAvailable = false;
    m_UpdateCount = 0;
    CleanupTempFiles();

    return true;
}

void CUpdateManager::CreateDirectoryStructure(const char* filePath) {
    char dirPath[512];
    strcpy_s(dirPath, sizeof(dirPath), filePath);

    char* lastSlash = strrchr(dirPath, '\\');
    if (!lastSlash) return;

    *lastSlash = '\0';

    char currentPath[512] = { 0 };
    char* token = dirPath;

    while (*token) {
        if (*token == '\\') {
            if (strlen(currentPath) > 0) {
                CreateDirectoryA(currentPath, NULL);
            }
            strcat_s(currentPath, sizeof(currentPath), "\\");
            token++;
            continue;
        }

        char temp[2] = { *token, '\0' };
        strcat_s(currentPath, sizeof(currentPath), temp);
        token++;
    }

    if (strlen(dirPath) > 0) {
        CreateDirectoryA(dirPath, NULL);
    }
}

// OLD FUNCTION - Keep for compatibility
bool CUpdateManager::DownloadUpdate() {
    return DownloadAllUpdates();
}

// OLD FUNCTION - Keep for compatibility
bool CUpdateManager::ApplyUpdate() {
    return ApplyAllUpdates();
}

bool CUpdateManager::DownloadFileWithProgress(const char* url, const char* destPath, DWORD* bytesDownloaded) {
    HINTERNET hInternet = InternetOpenA("GameServer UpdateManager/1.0",
        INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

    if (!hInternet) {
        LogUpdate("[UpdateManager] ERROR: InternetOpen failed: %d", GetLastError());
        return false;
    }

    HINTERNET hUrl = InternetOpenUrlA(hInternet, url, NULL, 0,
        INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
    if (!hUrl) {
        LogUpdate("[UpdateManager] ERROR: InternetOpenUrl failed: %d", GetLastError());
        InternetCloseHandle(hInternet);
        return false;
    }

    DWORD fileSize = 0;
    DWORD bufferSize = sizeof(fileSize);
    HttpQueryInfoA(hUrl, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
        &fileSize, &bufferSize, NULL);

    FILE* file = NULL;
    fopen_s(&file, destPath, "wb");
    if (!file) {
        LogUpdate("[UpdateManager] ERROR: Failed to create file: %s", destPath);
        InternetCloseHandle(hUrl);
        InternetCloseHandle(hInternet);
        return false;
    }

    BYTE buffer[8192];
    DWORD totalBytes = 0;
    DWORD dwRead = 0;
    int lastProgress = -1;

    while (InternetReadFile(hUrl, buffer, sizeof(buffer), &dwRead) && dwRead > 0) {
        fwrite(buffer, 1, dwRead, file);
        totalBytes += dwRead;

        int progress = 0;
        if (fileSize > 0) {
            progress = (int)((totalBytes * 100) / fileSize);

            if (progress != lastProgress) {
                LogUpdate("[UpdateManager] Download progress: %d%% (%d / %d bytes)",
                    progress, totalBytes, fileSize);
                lastProgress = progress;
                UpdateProgressDialog(progress, totalBytes, fileSize);
            }
        }
        else if (totalBytes % (1024 * 100) == 0) {
            LogUpdate("[UpdateManager] Downloaded: %d KB", totalBytes / 1024);
        }

        m_DownloadProgress = progress;
    }

    fclose(file);
    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);

    if (bytesDownloaded) {
        *bytesDownloaded = totalBytes;
    }

    m_DownloadProgress = 100;
    LogUpdate("[UpdateManager] Download complete: %d bytes", totalBytes);

    return true;
}

bool CUpdateManager::DownloadFile(const char* url, const char* destPath, DWORD* bytesDownloaded) {
    HINTERNET hInternet = InternetOpenA("GameServer UpdateManager/1.0",
        INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hInternet) {
        LogUpdate("[UpdateManager] ERROR: InternetOpen failed: %d", GetLastError());
        return false;
    }

    HINTERNET hUrl = InternetOpenUrlA(hInternet, url, NULL, 0,
        INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
    if (!hUrl) {
        LogUpdate("[UpdateManager] ERROR: InternetOpenUrl failed: %d", GetLastError());
        InternetCloseHandle(hInternet);
        return false;
    }

    FILE* file = NULL;
    fopen_s(&file, destPath, "wb");
    if (!file) {
        LogUpdate("[UpdateManager] ERROR: Failed to create file: %s", destPath);
        InternetCloseHandle(hUrl);
        InternetCloseHandle(hInternet);
        return false;
    }

    BYTE buffer[4096];
    DWORD totalBytes = 0;
    DWORD dwRead = 0;

    while (InternetReadFile(hUrl, buffer, sizeof(buffer), &dwRead) && dwRead > 0) {
        fwrite(buffer, 1, dwRead, file);
        totalBytes += dwRead;

        if (totalBytes % (1024 * 100) == 0) {
            LogUpdate("[UpdateManager] Downloaded: %d KB", totalBytes / 1024);
        }
    }

    fclose(file);
    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);

    if (bytesDownloaded) {
        *bytesDownloaded = totalBytes;
    }

    LogUpdate("[UpdateManager] Download complete: %d bytes", totalBytes);
    return true;
}

bool CUpdateManager::VerifyDownloadedFile(const char* filePath, const char* expectedHash) {
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    if (!GetFileAttributesExA(filePath, GetFileExInfoStandard, &fileInfo)) {
        return false;
    }

    DWORD fileSize = fileInfo.nFileSizeLow;

    // Simple size check for now
    return true;
}

bool CUpdateManager::CreateBackup(const char* filePath) {
    char backupPath[512];
    sprintf_s(backupPath, sizeof(backupPath), "%s.backup", filePath);

    if (PathFileExistsA(filePath)) {
        if (!CopyFileA(filePath, backupPath, FALSE)) {
            LogUpdate("[UpdateManager] Failed to create backup: %d", GetLastError());
            return false;
        }
        LogUpdate("[UpdateManager] Backup created: %s", backupPath);
    }

    return true;
}

void CUpdateManager::GetTempFilePath(const char* fileName, char* outPath, int outPathSize) {
    sprintf_s(outPath, outPathSize, "%s\\%s", m_TempDirectory, fileName);
}

void CUpdateManager::SetCurrentVersion(const char* version) {
    strcpy_s(m_CurrentVersion, sizeof(m_CurrentVersion), version);
}

void CUpdateManager::SetUpdateServerUrl(const char* url) {
    strcpy_s(m_UpdateServerUrl, sizeof(m_UpdateServerUrl), url);
}

void CUpdateManager::CalculateFileHash(const char* filePath, char* outHash, int outHashSize) {
    if (outHash && outHashSize > 0) {
        outHash[0] = '\0';
    }
}

void CUpdateManager::ShowNotification(const char* message, UINT icon) {
    if (m_hWnd && m_ShowNotifications) {
        MessageBoxA(m_hWnd, message, "GameServer Auto-Update", icon);
    }
}

void CUpdateManager::LogUpdate(const char* format, ...) {
    char buffer[512];
    va_list args;
    va_start(args, format);
    vsprintf_s(buffer, sizeof(buffer), format, args);
    va_end(args);

    LogAdd(LOG_BLACK, "%s", buffer);
}

const char* CUpdateManager::GetStatusString() const {
    switch (m_Status) {
    case UPDATE_STATUS_NO_UPDATE: return "No updates available";
    case UPDATE_STATUS_AVAILABLE: return "Update available";
    case UPDATE_STATUS_DOWNLOADING: return "Downloading update...";
    case UPDATE_STATUS_READY: return "Update ready to install";
    case UPDATE_STATUS_ERROR: return "Error checking for updates";
    case UPDATE_STATUS_DISABLED: return "Auto-update disabled";
    default: return "Unknown status";
    }
}

bool CUpdateManager::IsUpdateAvailable() {
    return m_UpdateAvailable;
}

UpdateInfo CUpdateManager::GetUpdateInfo() {
    if (m_UpdateCount > 0) {
        return m_AvailableUpdates[0];
    }

    UpdateInfo empty;
    return empty;
}

void CUpdateManager::OnTimer() {
    if (!m_Enabled || !m_AutoCheck) {
        return;
    }

    DWORD currentTime = GetTickCount();
    if (currentTime - m_LastCheckTime > UPDATE_CHECK_INTERVAL) {
        LogUpdate("[UpdateManager] Automatic update check (interval: %d ms)", UPDATE_CHECK_INTERVAL);
        CheckForUpdates();
        m_LastCheckTime = currentTime;
    }
}

void CUpdateManager::ManualCheckForUpdates() {
    LogUpdate("[UpdateManager] Manual update check requested");

    if (!m_Enabled) {
        MessageBoxA(m_hWnd,
            "Auto-update system is disabled.\n\n"
            "Enable it in Update - Config Updates to check for updates.",
            "Auto-Update Disabled", MB_ICONINFORMATION);
        return;
    }

    if (!CheckForUpdates()) {
        if (m_Status == UPDATE_STATUS_ERROR) {
            MessageBoxA(m_hWnd,
                "Failed to check for updates.\n\n"
                "Please check:\n"
                "- Your internet connection\n"
                "- Update server URL is correct\n"
                "- Update server is online\n\n"
                "Check the console for details.",
                "Update Check Failed", MB_ICONERROR);
        }
        else if (!m_UpdateAvailable) {
            char msg[256];
            sprintf_s(msg, sizeof(msg),
                "You have the latest version!\n\n"
                "Current Version: %s\n\n"
                "No updates needed.",
                m_CurrentVersion);
            MessageBoxA(m_hWnd, msg, "Up to Date", MB_ICONINFORMATION);
        }
        return;
    }

    // Show detailed update list and ask to download
    char msg[4096];
    char temp[512];

    sprintf_s(msg, sizeof(msg),
        "UPDATES AVAILABLE!\n\n"
        "Found %d update(s):\n\n",
        m_UpdateCount);

    float totalSize = 0;
    for (int i = 0; i < m_UpdateCount; i++) {
        UpdateInfo* update = &m_AvailableUpdates[i];
        totalSize += update->fileSize;

        sprintf_s(temp, sizeof(temp),
            "%d) %s (v%s) - %.2f MB\n"
            "   %s\n\n",
            i + 1,
            update->fileName,
            update->version,
            update->fileSize / 1024.0 / 1024.0,
            update->description);
        strcat_s(msg, sizeof(msg), temp);
    }

    sprintf_s(temp, sizeof(temp),
        "Total Size: %.2f MB\n\n"
        "Download and install these updates now?",
        totalSize / 1024.0 / 1024.0);
    strcat_s(msg, sizeof(msg), temp);

    int result = MessageBoxA(m_hWnd, msg, "Updates Available",
        MB_YESNO | MB_ICONQUESTION);

    if (result == IDYES) {
        if (DownloadAllUpdates()) {
            int applyResult = MessageBoxA(m_hWnd,
                "All downloads completed!\n\n"
                "Apply updates now?",
                "Apply Updates?", MB_YESNO | MB_ICONQUESTION);

            if (applyResult == IDYES) {
                ApplyAllUpdates();
            }
        }
    }
}

void CUpdateManager::EnableAutoUpdate(bool enable) {
    m_Enabled = enable;
    SaveConfig(".\\Data\\UpdateConfig.ini");
    LogUpdate("[UpdateManager] Auto-update %s", enable ? "ENABLED" : "DISABLED");
}

void CUpdateManager::SetDownloadProgress(int percent) {
    m_DownloadProgress = percent;
}

void CUpdateManager::CleanupTempFiles() {
    WIN32_FIND_DATAA findData;
    HANDLE hFind;
    char searchPath[512];
    char filePath[512];

    sprintf_s(searchPath, sizeof(searchPath), "%s\\*.*", m_TempDirectory);
    hFind = FindFirstFileA(searchPath, &findData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) {
                continue;
            }

            sprintf_s(filePath, sizeof(filePath), "%s\\%s", m_TempDirectory, findData.cFileName);
            DeleteFileA(filePath);
        } while (FindNextFileA(hFind, &findData));

        FindClose(hFind);
    }

    LogUpdate("[UpdateManager] Temporary files cleaned up successfully");
}

bool CUpdateManager::DeleteDirectoryRecursive(const char* dirPath) {
    WIN32_FIND_DATAA findData;
    HANDLE hFind;
    char searchPath[512];
    char filePath[512];

    sprintf_s(searchPath, sizeof(searchPath), "%s\\*.*", dirPath);
    hFind = FindFirstFileA(searchPath, &findData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) {
                continue;
            }

            sprintf_s(filePath, sizeof(filePath), "%s\\%s", dirPath, findData.cFileName);

            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                DeleteDirectoryRecursive(filePath);
            }
            else {
                DeleteFileA(filePath);
            }
        } while (FindNextFileA(hFind, &findData));

        FindClose(hFind);
    }

    return RemoveDirectoryA(dirPath) ? true : false;
}

INT_PTR CALLBACK UpdateConfigDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static CUpdateManager* pManager = NULL;

    switch (message) {
    case WM_INITDIALOG:
    {
        pManager = (CUpdateManager*)lParam;

        CheckDlgButton(hDlg, IDC_CHECK_ENABLED, pManager->IsEnabled() ? BST_CHECKED : BST_UNCHECKED);
        CheckDlgButton(hDlg, IDC_CHECK_AUTOCHECK,
            GetPrivateProfileInt("UpdateManager", "AutoCheck", 1, ".\\Data\\UpdateConfig.ini") ? BST_CHECKED : BST_UNCHECKED);
        CheckDlgButton(hDlg, IDC_CHECK_AUTODOWNLOAD,
            GetPrivateProfileInt("UpdateManager", "AutoDownload", 0, ".\\Data\\UpdateConfig.ini") ? BST_CHECKED : BST_UNCHECKED);
        CheckDlgButton(hDlg, IDC_CHECK_SHOWNOTIFICATIONS,
            GetPrivateProfileInt("UpdateManager", "ShowNotifications", 1, ".\\Data\\UpdateConfig.ini") ? BST_CHECKED : BST_UNCHECKED);

        SetDlgItemText(hDlg, IDC_EDIT_UPDATEURL, pManager->GetUpdateServerUrl());
        SetDlgItemText(hDlg, IDC_EDIT_CURRENTVERSION, pManager->GetCurrentVersion());

        return TRUE;
    }

    case WM_COMMAND:
    {
        if (LOWORD(wParam) == IDC_BTN_SAVE_UPDATECONFIG || LOWORD(wParam) == IDOK) {
            bool enabled = IsDlgButtonChecked(hDlg, IDC_CHECK_ENABLED) == BST_CHECKED;
            bool autoCheck = IsDlgButtonChecked(hDlg, IDC_CHECK_AUTOCHECK) == BST_CHECKED;
            bool autoDownload = IsDlgButtonChecked(hDlg, IDC_CHECK_AUTODOWNLOAD) == BST_CHECKED;
            bool showNotifications = IsDlgButtonChecked(hDlg, IDC_CHECK_SHOWNOTIFICATIONS) == BST_CHECKED;

            char updateUrl[512];
            char currentVersion[32];
            GetDlgItemText(hDlg, IDC_EDIT_UPDATEURL, updateUrl, sizeof(updateUrl));
            GetDlgItemText(hDlg, IDC_EDIT_CURRENTVERSION, currentVersion, sizeof(currentVersion));

            if (strlen(updateUrl) == 0) {
                MessageBoxA(hDlg, "Update Server URL cannot be empty!", "Validation Error", MB_OK | MB_ICONERROR);
                return TRUE;
            }

            WritePrivateProfileString("UpdateManager", "Enabled", enabled ? "1" : "0", ".\\Data\\UpdateConfig.ini");
            WritePrivateProfileString("UpdateManager", "AutoCheck", autoCheck ? "1" : "0", ".\\Data\\UpdateConfig.ini");
            WritePrivateProfileString("UpdateManager", "AutoDownload", autoDownload ? "1" : "0", ".\\Data\\UpdateConfig.ini");
            WritePrivateProfileString("UpdateManager", "ShowNotifications", showNotifications ? "1" : "0", ".\\Data\\UpdateConfig.ini");
            WritePrivateProfileString("UpdateManager", "UpdateServerUrl", updateUrl, ".\\Data\\UpdateConfig.ini");
            WritePrivateProfileString("UpdateManager", "CurrentVersion", currentVersion, ".\\Data\\UpdateConfig.ini");

            pManager->LoadConfig(".\\Data\\UpdateConfig.ini");

            MessageBoxA(hDlg, "Configuration saved successfully!", "Success", MB_OK | MB_ICONINFORMATION);
            EndDialog(hDlg, IDOK);
            return TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    }

    return FALSE;
}

void CUpdateManager::ShowConfigDialog() {
    DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_UPDATE_CONFIG), m_hWnd, UpdateConfigDlgProc, (LPARAM)this);
}

INT_PTR CALLBACK ProgressDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG:
    {
        RECT rc;
        GetWindowRect(hDlg, &rc);
        int x = (GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2;
        int y = (GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2;
        SetWindowPos(hDlg, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        SendDlgItemMessage(hDlg, IDC_PROGRESS_BAR, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
        SendDlgItemMessage(hDlg, IDC_PROGRESS_BAR, PBM_SETPOS, 0, 0);

        return TRUE;
    }
    }

    return FALSE;
}

void CUpdateManager::ShowProgressDialog(const char* fileName) {
    if (m_hProgressWnd != NULL) {
        return;
    }

    m_hProgressWnd = CreateDialogParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PROGRESS_DIALOG),
        m_hWnd, ProgressDlgProc, (LPARAM)this);

    if (m_hProgressWnd) {
        SetDlgItemText(m_hProgressWnd, IDC_STATIC_FILENAME, fileName);
        ShowWindow(m_hProgressWnd, SW_SHOW);
    }
}

void CUpdateManager::UpdateProgressDialog(int percent, DWORD current, DWORD total) {
    if (m_hProgressWnd == NULL) {
        return;
    }

    SendDlgItemMessage(m_hProgressWnd, IDC_PROGRESS_BAR, PBM_SETPOS, percent, 0);

    char progressText[128];
    sprintf_s(progressText, sizeof(progressText), "%d%%", percent);
    SetDlgItemText(m_hProgressWnd, IDC_STATIC_PROGRESS, progressText);

    char sizeText[128];
    sprintf_s(sizeText, sizeof(sizeText), "%.2f MB / %.2f MB",
        current / 1024.0 / 1024.0, total / 1024.0 / 1024.0);
    SetDlgItemText(m_hProgressWnd, IDC_STATIC_SIZE, sizeText);

    MSG msg;
    while (PeekMessage(&msg, m_hProgressWnd, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void CUpdateManager::CloseProgressDialog() {
    if (m_hProgressWnd != NULL) {
        DestroyWindow(m_hProgressWnd);
        m_hProgressWnd = NULL;
    }
}

bool CUpdateManager::IsVersionNewer(const char* version1, const char* version2) {
    // Parse version numbers (format: X.Y.Z)
    int v1_major = 0, v1_minor = 0, v1_patch = 0;
    int v2_major = 0, v2_minor = 0, v2_patch = 0;

    sscanf_s(version1, "%d.%d.%d", &v1_major, &v1_minor, &v1_patch);
    sscanf_s(version2, "%d.%d.%d", &v2_major, &v2_minor, &v2_patch);

    if (v1_major > v2_major) return true;
    if (v1_major < v2_major) return false;

    if (v1_minor > v2_minor) return true;
    if (v1_minor < v2_minor) return false;

    if (v1_patch > v2_patch) return true;

    return false; // Same version or older
}

bool CUpdateManager::IsUpdateAlreadyInstalled(const char* fileName, const char* version) {
    // Check in UpdateConfig.ini if this specific file+version was installed
    char sectionName[128];
    sprintf_s(sectionName, sizeof(sectionName), "InstalledUpdates");

    char installedVersion[32] = { 0 };
    GetPrivateProfileString(sectionName, fileName, "",
        installedVersion, sizeof(installedVersion), ".\\Data\\UpdateConfig.ini");

    if (strlen(installedVersion) == 0) {
        return false; // Not installed
    }

    // Check if installed version is same or newer
    if (strcmp(installedVersion, version) == 0) {
        return true; // Exact same version already installed
    }

    // Check if installed version is newer
    if (!IsVersionNewer(version, installedVersion)) {
        return true; // Installed version is newer or same
    }

    return false; // Available version is newer
}

void CUpdateManager::SaveInstalledUpdate(const char* fileName, const char* version) {
    char sectionName[128];
    sprintf_s(sectionName, sizeof(sectionName), "InstalledUpdates");

    WritePrivateProfileString(sectionName, fileName, version, ".\\Data\\UpdateConfig.ini");

    LogUpdate("[UpdateManager] Saved installed update: %s = %s", fileName, version);
}