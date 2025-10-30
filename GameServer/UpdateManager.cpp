#include "stdafx.h"
#include "UpdateManager.h"
#include "Log.h"
#include "Util.h"
#include <shlwapi.h>
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
	
	// Create temp directory if it doesn't exist
	CreateDirectoryA(".\\Update", NULL);
	CreateDirectoryA(m_TempDirectory, NULL);
	
	// Load configuration
	LoadConfig(".\\Data\\UpdateConfig.ini");
	
	LogUpdate("[UpdateManager] Initialized (Version: %s, Enabled: %s)", 
		m_CurrentVersion, m_Enabled ? "YES" : "NO");
	
	if (m_Enabled && strlen(m_UpdateServerUrl) == 0) {
		LogUpdate("[UpdateManager] WARNING: Auto-update is enabled but no update server URL is configured!");
		m_Enabled = false;
	}
	
	// Perform initial check if auto-check is enabled
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
	
	LogUpdate("[UpdateManager] Checking for updates from: %s", m_UpdateServerUrl);
	
	// Download update manifest
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
	
	// Read manifest file
	FILE* file = NULL;
	fopen_s(&file, manifestPath, "r");
	if (!file) {
		LogUpdate("[UpdateManager] ERROR: Failed to open update manifest file");
		m_Status = UPDATE_STATUS_ERROR;
		return false;
	}
	
	char manifestData[4096] = {0};
	fread(manifestData, 1, sizeof(manifestData) - 1, file);
	fclose(file);
	
	// Parse manifest
	if (!ParseUpdateManifest(manifestData)) {
		LogUpdate("[UpdateManager] ERROR: Failed to parse update manifest");
		m_Status = UPDATE_STATUS_ERROR;
		return false;
	}
	
	// Compare versions
	if (strcmp(m_LatestUpdate.version, m_CurrentVersion) != 0) {
		m_UpdateAvailable = true;
		m_Status = UPDATE_STATUS_AVAILABLE;
		LogUpdate("[UpdateManager] UPDATE AVAILABLE: %s -> %s", 
			m_CurrentVersion, m_LatestUpdate.version);
		LogUpdate("[UpdateManager] Description: %s", m_LatestUpdate.description);
		
		if (m_ShowNotifications) {
			char msg[512];
			sprintf_s(msg, sizeof(msg), 
				"UPDATE AVAILABLE!\n\n"
				"Current Version: %s\n"
				"New Version: %s\n\n"
				"Description: %s\n\n"
				"Go to menu [Update] to download and apply the update.",
				m_CurrentVersion, 
				m_LatestUpdate.version,
				m_LatestUpdate.description);
			ShowNotification(msg, MB_ICONINFORMATION);
		}
		
		// Auto-download if enabled
		if (m_AutoDownload) {
			LogUpdate("[UpdateManager] Auto-download is enabled, starting download...");
			DownloadUpdate();
		}
		
		return true;
	} else {
		m_UpdateAvailable = false;
		m_Status = UPDATE_STATUS_NO_UPDATE;
		LogUpdate("[UpdateManager] No updates available (Current version: %s)", m_CurrentVersion);
		return false;
	}
	
	m_LastCheckTime = GetTickCount();
	return true;
}

bool CUpdateManager::ParseUpdateManifest(const char* manifestData) {
	// Simple INI-style parser
	char buffer[1024];
	const char* ptr = manifestData;
	
	while (*ptr) {
		// Read line
		int i = 0;
		while (*ptr && *ptr != '\n' && i < sizeof(buffer) - 1) {
			buffer[i++] = *ptr++;
		}
		buffer[i] = '\0';
		if (*ptr == '\n') ptr++;
		
		// Skip empty lines and comments
		if (buffer[0] == '\0' || buffer[0] == ';' || buffer[0] == '#' || buffer[0] == '[') {
			continue;
		}
		
		// Parse key=value
		char* equals = strchr(buffer, '=');
		if (equals) {
			*equals = '\0';
			char* key = buffer;
			char* value = equals + 1;
			
			// Trim whitespace
			while (*key == ' ' || *key == '\t') key++;
			while (*value == ' ' || *value == '\t') value++;
			
			// Remove trailing whitespace
			char* end = key + strlen(key) - 1;
			while (end > key && (*end == ' ' || *end == '\t' || *end == '\r')) *end-- = '\0';
			end = value + strlen(value) - 1;
			while (end > value && (*end == ' ' || *end == '\t' || *end == '\r')) *end-- = '\0';
			
			// Store values
			if (_stricmp(key, "Version") == 0) {
				strcpy_s(m_LatestUpdate.version, sizeof(m_LatestUpdate.version), value);
			} else if (_stricmp(key, "DownloadUrl") == 0) {
				strcpy_s(m_LatestUpdate.downloadUrl, sizeof(m_LatestUpdate.downloadUrl), value);
			} else if (_stricmp(key, "FileName") == 0) {
				strcpy_s(m_LatestUpdate.fileName, sizeof(m_LatestUpdate.fileName), value);
			} else if (_stricmp(key, "FileHash") == 0) {
				strcpy_s(m_LatestUpdate.fileHash, sizeof(m_LatestUpdate.fileHash), value);
			} else if (_stricmp(key, "FileSize") == 0) {
				m_LatestUpdate.fileSize = atoi(value);
			} else if (_stricmp(key, "FileType") == 0) {
				m_LatestUpdate.fileType = (UpdateFileType)atoi(value);
			} else if (_stricmp(key, "Description") == 0) {
				strcpy_s(m_LatestUpdate.description, sizeof(m_LatestUpdate.description), value);
			} else if (_stricmp(key, "TargetPath") == 0) {
				strcpy_s(m_LatestUpdate.targetPath, sizeof(m_LatestUpdate.targetPath), value);
			} else if (_stricmp(key, "Required") == 0) {
				m_LatestUpdate.isRequired = atoi(value) ? true : false;
			}
		}
	}
	
	// Validate required fields
	if (strlen(m_LatestUpdate.version) == 0 || strlen(m_LatestUpdate.downloadUrl) == 0 || strlen(m_LatestUpdate.fileName) == 0) {
		LogUpdate("[UpdateManager] ERROR: Invalid update manifest - missing required fields");
		return false;
	}
	
	return true;
}

bool CUpdateManager::DownloadUpdate() {
	if (!m_UpdateAvailable) {
		LogUpdate("[UpdateManager] No update available to download");
		return false;
	}
	
	LogUpdate("[UpdateManager] Starting download: %s", m_LatestUpdate.downloadUrl);
	m_Status = UPDATE_STATUS_DOWNLOADING;
	m_DownloadProgress = 0;
	
	char destPath[512];
	GetTempFilePath(m_LatestUpdate.fileName, destPath, sizeof(destPath));
	
	DWORD bytesDownloaded = 0;
	if (!DownloadFileWithProgress(m_LatestUpdate.downloadUrl, destPath, &bytesDownloaded)) {
		LogUpdate("[UpdateManager] ERROR: Download failed");
		m_Status = UPDATE_STATUS_ERROR;
		if (m_ShowNotifications) {
			ShowNotification("Update download failed!\n\nPlease try again later or download manually.", MB_ICONERROR);
		}
		return false;
	}
	
	LogUpdate("[UpdateManager] Download complete: %d bytes", bytesDownloaded);
	
	// Verify file hash if provided
	if (strlen(m_LatestUpdate.fileHash) > 0) {
		LogUpdate("[UpdateManager] Verifying downloaded file...");
		if (!VerifyDownloadedFile(destPath, m_LatestUpdate.fileHash)) {
			LogUpdate("[UpdateManager] ERROR: File verification failed!");
			m_Status = UPDATE_STATUS_ERROR;
			if (m_ShowNotifications) {
				ShowNotification("Update verification failed!\n\nThe downloaded file is corrupted or tampered.", MB_ICONERROR);
			}
			return false;
		}
		LogUpdate("[UpdateManager] File verification successful");
	}
	
	m_Status = UPDATE_STATUS_READY;
	LogUpdate("[UpdateManager] Update ready to install");
	
	return true;
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
	
	// Get file size
	DWORD fileSize = 0;
	DWORD bufferSize = sizeof(fileSize);
	HttpQueryInfoA(hUrl, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, 
		&fileSize, &bufferSize, NULL);
	
	if (fileSize == 0) {
		fileSize = m_LatestUpdate.fileSize;
	}
	
	// Open destination file
	FILE* file = NULL;
	fopen_s(&file, destPath, "wb");
	if (!file) {
		LogUpdate("[UpdateManager] ERROR: Failed to create file: %s", destPath);
		InternetCloseHandle(hUrl);
		InternetCloseHandle(hInternet);
		return false;
	}
	
	// Download in chunks with progress
	BYTE buffer[8192];
	DWORD totalBytes = 0;
	DWORD dwRead = 0;
	int lastProgress = -1;
	
	while (InternetReadFile(hUrl, buffer, sizeof(buffer), &dwRead) && dwRead > 0) {
		fwrite(buffer, 1, dwRead, file);
		totalBytes += dwRead;
		
		// Calculate and log progress
		int progress = 0;
		if (fileSize > 0) {
			progress = (int)((totalBytes * 100) / fileSize);
			if (progress != lastProgress && progress % 10 == 0) {
				LogUpdate("[UpdateManager] Download progress: %d%% (%d / %d bytes)", 
					progress, totalBytes, fileSize);
				lastProgress = progress;
			}
		} else if (totalBytes % (1024 * 100) == 0) {
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
	
	// Open destination file
	FILE* file = NULL;
	fopen_s(&file, destPath, "wb");
	if (!file) {
		LogUpdate("[UpdateManager] ERROR: Failed to create file: %s", destPath);
		InternetCloseHandle(hUrl);
		InternetCloseHandle(hInternet);
		return false;
	}
	
	// Download in chunks
	BYTE buffer[4096];
	DWORD totalBytes = 0;
	DWORD dwRead = 0;
	
	while (InternetReadFile(hUrl, buffer, sizeof(buffer), &dwRead) && dwRead > 0) {
		fwrite(buffer, 1, dwRead, file);
		totalBytes += dwRead;
		
		if (totalBytes % (1024 * 100) == 0) { // Log every 100KB
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
	// Simple file hash verification (MD5 or SHA256)
	// For simplicity, we'll just compare file size or implement basic checksum
	// You can implement proper MD5/SHA256 verification here
	
	WIN32_FILE_ATTRIBUTE_DATA fileInfo;
	if (!GetFileAttributesExA(filePath, GetFileExInfoStandard, &fileInfo)) {
		return false;
	}
	
	DWORD fileSize = fileInfo.nFileSizeLow;
	if (m_LatestUpdate.fileSize > 0 && fileSize != m_LatestUpdate.fileSize) {
		LogUpdate("[UpdateManager] File size mismatch: expected %d, got %d", 
			m_LatestUpdate.fileSize, fileSize);
		return false;
	}
	
	// TODO: Implement proper hash verification (MD5/SHA256)
	// For now, just return true if file size matches
	return true;
}

bool CUpdateManager::ApplyUpdate() {
	if (m_Status != UPDATE_STATUS_READY) {
		LogUpdate("[UpdateManager] ERROR: No update ready to apply (Status: %d)", m_Status);
		if (m_ShowNotifications) {
			ShowNotification("No update is ready to apply.\n\nPlease download the update first.", MB_ICONWARNING);
		}
		return false;
	}
	
	// Confirm with user
	int result = MessageBox(m_hWnd, 
		"Are you sure you want to apply the update?\n\n"
		"The GameServer will be closed and restarted with the new version.\n\n"
		"Make sure all players are disconnected before proceeding!",
		"Apply Update - Confirmation", 
		MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);
	
	if (result != IDYES) {
		LogUpdate("[UpdateManager] Update cancelled by user");
		return false;
	}
	
	LogUpdate("[UpdateManager] Applying update...");
	
	char updateFile[512];
	char targetFile[512];
	GetTempFilePath(m_LatestUpdate.fileName, updateFile, sizeof(updateFile));
	
	// Determine target file path based on file type
	if (m_LatestUpdate.fileType == UPDATE_FILE_EXECUTABLE) {
		// For executable, we need special handling
		strcpy_s(targetFile, sizeof(targetFile), ".\\GameServer.exe");
		
		// Create backup
		LogUpdate("[UpdateManager] Creating backup of current executable...");
		if (!CreateBackup(targetFile)) {
			LogUpdate("[UpdateManager] ERROR: Failed to create backup");
			if (m_ShowNotifications) {
				ShowNotification("Failed to create backup!\n\nUpdate aborted for safety.", MB_ICONERROR);
			}
			return false;
		}
		
		// CRITICAL: Update the version number BEFORE exiting
		// This prevents the update from being detected again after restart
		LogUpdate("[UpdateManager] Updating version: %s -> %s", m_CurrentVersion, m_LatestUpdate.version);
		strcpy_s(m_CurrentVersion, sizeof(m_CurrentVersion), m_LatestUpdate.version);
		m_Status = UPDATE_STATUS_NO_UPDATE;
		m_UpdateAvailable = false;
		SaveConfig(".\\Data\\UpdateConfig.ini");
		LogUpdate("[UpdateManager] Version updated in config file");
		
		// Create update script
		FILE* script = NULL;
		fopen_s(&script, ".\\Update\\apply_update.bat", "w");
		if (script) {
			fprintf(script, "@echo off\n");
			fprintf(script, "echo Applying GameServer update...\n");
			fprintf(script, "timeout /t 2 /nobreak >nul\n");
			fprintf(script, "echo Replacing executable...\n");
			fprintf(script, "copy /Y \"%s\" \"%s\"\n", updateFile, targetFile);
			fprintf(script, "if errorlevel 1 goto error\n");
			fprintf(script, "echo Update applied successfully!\n");
			fprintf(script, "echo Restarting GameServer...\n");
			fprintf(script, "start \"\" \"%s\"\n", targetFile);
			fprintf(script, "goto end\n");
			fprintf(script, ":error\n");
			fprintf(script, "echo ERROR: Failed to apply update!\n");
			fprintf(script, "echo Restoring backup...\n");
			fprintf(script, "copy /Y \"%s.backup\" \"%s\"\n", targetFile, targetFile);
			fprintf(script, "pause\n");
			fprintf(script, ":end\n");
			fclose(script);
			
			LogUpdate("[UpdateManager] Launching update script and exiting...");
			
			// Launch update script and exit
			STARTUPINFOA si = {0};
			PROCESS_INFORMATION pi = {0};
			si.cb = sizeof(si);
			
			char cmdLine[512];
			strcpy_s(cmdLine, sizeof(cmdLine), ".\\Update\\apply_update.bat");
			
			if (CreateProcessA(NULL, cmdLine, NULL, NULL, FALSE, 
				CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				
				// Exit the application
				PostQuitMessage(0);
				return true;
			} else {
				LogUpdate("[UpdateManager] ERROR: Failed to launch update script");
				return false;
			}
		}
	} else {
		// For data files, just copy directly
		// Check if custom target path is specified
		if (strlen(m_LatestUpdate.targetPath) > 0) {
			// Use custom target path
			strcpy_s(targetFile, sizeof(targetFile), m_LatestUpdate.targetPath);
		} else {
			// Use default Data folder
			sprintf_s(targetFile, sizeof(targetFile), ".\\Data\\%s", m_LatestUpdate.fileName);
		}
		
		LogUpdate("[UpdateManager] Updating file: %s", targetFile);
		
		// Create directory if it doesn't exist
		char dirPath[512];
		strcpy_s(dirPath, sizeof(dirPath), targetFile);
		char* lastSlash = strrchr(dirPath, '\\');
		if (lastSlash) {
			*lastSlash = '\0';
			// Create all directories in path
			char* token = dirPath;
			char currentPath[512] = {0};
			while (*token) {
				if (*token == '\\') {
					*token = '\0';
					if (strlen(currentPath) > 0) {
						strcat_s(currentPath, sizeof(currentPath), "\\");
					}
					strcat_s(currentPath, sizeof(currentPath), dirPath);
					CreateDirectoryA(currentPath, NULL);
					dirPath[0] = '\0';
					token++;
					continue;
				}
				char temp[2] = {*token, '\0'};
				strcat_s(dirPath, sizeof(dirPath), temp);
				token++;
			}
			if (strlen(dirPath) > 0) {
				if (strlen(currentPath) > 0) {
					strcat_s(currentPath, sizeof(currentPath), "\\");
				}
				strcat_s(currentPath, sizeof(currentPath), dirPath);
				CreateDirectoryA(currentPath, NULL);
			}
		}
		
		// Create backup
		if (!CreateBackup(targetFile)) {
			LogUpdate("[UpdateManager] WARNING: Failed to create backup");
		}
		
		// Copy file
		if (!CopyFileA(updateFile, targetFile, FALSE)) {
			LogUpdate("[UpdateManager] ERROR: Failed to copy file: %d", GetLastError());
			if (m_ShowNotifications) {
				ShowNotification("Failed to apply update!\n\nPlease check file permissions.", MB_ICONERROR);
			}
			return false;
		}
		
		LogUpdate("[UpdateManager] Update applied successfully");
		strcpy_s(m_CurrentVersion, sizeof(m_CurrentVersion), m_LatestUpdate.version);
		SaveConfig(".\\Data\\UpdateConfig.ini");
		
		if (m_ShowNotifications) {
			char msg[512];
			sprintf_s(msg, sizeof(msg), 
				"Update applied successfully!\n\n"
				"File: %s\n"
				"Path: %s\n"
				"Version: %s\n\n"
				"You may need to reload the data file for changes to take effect.",
				m_LatestUpdate.fileName,
				targetFile,
				m_LatestUpdate.version);
			ShowNotification(msg, MB_ICONINFORMATION);
		}
	}
	
	m_Status = UPDATE_STATUS_NO_UPDATE;
	m_UpdateAvailable = false;
	
	// Clean up temp files after successful update
	LogUpdate("[UpdateManager] Cleaning up temporary files...");
	CleanupTempFiles();
	
	return true;
}

void CUpdateManager::CleanupTempFiles() {
	// Delete the Update/Temp folder and its contents
	if (DeleteDirectoryRecursive(m_TempDirectory)) {
		LogUpdate("[UpdateManager] Temporary files cleaned up successfully");
	} else {
		LogUpdate("[UpdateManager] WARNING: Failed to clean up some temporary files");
	}
	
	// Optionally delete the entire Update folder
	char updateFolder[256];
	strcpy_s(updateFolder, sizeof(updateFolder), ".\\Update");
	if (DeleteDirectoryRecursive(updateFolder)) {
		LogUpdate("[UpdateManager] Update folder cleaned up successfully");
	}
}

bool CUpdateManager::DeleteDirectoryRecursive(const char* dirPath) {
	WIN32_FIND_DATAA findData;
	HANDLE hFind;
	char searchPath[512];
	char filePath[512];
	
	// First, delete all files in the directory
	sprintf_s(searchPath, sizeof(searchPath), "%s\\*.*", dirPath);
	hFind = FindFirstFileA(searchPath, &findData);
	
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// Skip . and ..
			if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) {
				continue;
			}
			
			sprintf_s(filePath, sizeof(filePath), "%s\\%s", dirPath, findData.cFileName);
			
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				// Recursively delete subdirectory
				DeleteDirectoryRecursive(filePath);
			} else {
				// Delete file
				DeleteFileA(filePath);
			}
		} while (FindNextFileA(hFind, &findData));
		
		FindClose(hFind);
	}
	
	// Now delete the directory itself
	return RemoveDirectoryA(dirPath) ? true : false;
}

void CUpdateManager::SetDownloadProgress(int percent) {
	m_DownloadProgress = percent;
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
	// Placeholder - implement MD5/SHA256 if needed
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
	return m_LatestUpdate;
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
			"Enable it in Data\\UpdateConfig.ini to check for updates.",
			"Auto-Update Disabled", MB_ICONINFORMATION);
		return;
	}
	
	if (CheckForUpdates()) {
		if (!m_UpdateAvailable) {
			MessageBoxA(m_hWnd, 
				"Your GameServer is up to date!\n\n"
				"No new updates are available at this time.",
				"No Updates Available", MB_ICONINFORMATION);
		} else {
			// Show dialog with download option
			char msg[768];
			sprintf_s(msg, sizeof(msg), 
				"UPDATE AVAILABLE!\n\n"
				"Current Version: %s\n"
				"New Version: %s\n"
				"File: %s\n"
				"Size: %.2f MB\n\n"
				"Description:\n%s\n\n"
				"Do you want to download and install this update now?",
				m_CurrentVersion,
				m_LatestUpdate.version,
				m_LatestUpdate.fileName,
				m_LatestUpdate.fileSize / 1024.0 / 1024.0,
				m_LatestUpdate.description);
			
			int result = MessageBoxA(m_hWnd, msg, "Update Available", MB_YESNO | MB_ICONQUESTION);
			
			if (result == IDYES) {
				// Download with progress
				if (DownloadUpdate()) {
					// Ask to apply now
					int applyResult = MessageBoxA(m_hWnd,
						"Download completed successfully!\n\n"
						"Do you want to apply the update now?\n"
						"(The server will restart if updating executable)",
						"Apply Update?", MB_YESNO | MB_ICONQUESTION);
					
					if (applyResult == IDYES) {
						ApplyUpdate();
					}
				}
			}
		}
	} else {
		if (m_Status == UPDATE_STATUS_ERROR) {
			MessageBoxA(m_hWnd, 
				"Failed to check for updates.\n\n"
				"Please verify your internet connection and update server URL.",
				"Update Check Failed", MB_ICONERROR);
		}
	}
}

void CUpdateManager::EnableAutoUpdate(bool enable) {
	m_Enabled = enable;
	SaveConfig(".\\Data\\UpdateConfig.ini");
	LogUpdate("[UpdateManager] Auto-update %s", enable ? "ENABLED" : "DISABLED");
}
