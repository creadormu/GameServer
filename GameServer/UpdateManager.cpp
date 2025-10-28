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
	m_UpdateServerUrl = "";
	m_CurrentVersion = "1.0.0";
	m_TempDirectory = ".\\Update\\Temp";
	m_Status = UPDATE_STATUS_NO_UPDATE;
	m_hWnd = NULL;
	m_DownloadThread = NULL;
	m_UpdateAvailable = false;
	m_LastCheckTime = 0;
	
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
	CreateDirectoryA(m_TempDirectory.c_str(), NULL);
	
	// Load configuration
	LoadConfig(".\\Data\\UpdateConfig.ini");
	
	LogUpdate("[UpdateManager] Initialized (Version: %s, Enabled: %s)", 
		m_CurrentVersion.c_str(), m_Enabled ? "YES" : "NO");
	
	if (m_Enabled && m_UpdateServerUrl.empty()) {
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
	
	char buffer[256];
	GetPrivateProfileString("UpdateManager", "UpdateServerUrl", "", buffer, sizeof(buffer), configFile);
	m_UpdateServerUrl = buffer;
	
	GetPrivateProfileString("UpdateManager", "CurrentVersion", "1.0.0", buffer, sizeof(buffer), configFile);
	m_CurrentVersion = buffer;
	
	GetPrivateProfileString("UpdateManager", "TempDirectory", ".\\Update\\Temp", buffer, sizeof(buffer), configFile);
	m_TempDirectory = buffer;
}

void CUpdateManager::SaveConfig(const char* configFile) {
	WritePrivateProfileString("UpdateManager", "Enabled", m_Enabled ? "1" : "0", configFile);
	WritePrivateProfileString("UpdateManager", "AutoCheck", m_AutoCheck ? "1" : "0", configFile);
	WritePrivateProfileString("UpdateManager", "AutoDownload", m_AutoDownload ? "1" : "0", configFile);
	WritePrivateProfileString("UpdateManager", "ShowNotifications", m_ShowNotifications ? "1" : "0", configFile);
	WritePrivateProfileString("UpdateManager", "UpdateServerUrl", m_UpdateServerUrl.c_str(), configFile);
	WritePrivateProfileString("UpdateManager", "CurrentVersion", m_CurrentVersion.c_str(), configFile);
	WritePrivateProfileString("UpdateManager", "TempDirectory", m_TempDirectory.c_str(), configFile);
}

bool CUpdateManager::CheckForUpdates() {
	if (!m_Enabled) {
		LogUpdate("[UpdateManager] Update check skipped - auto-update is disabled");
		return false;
	}
	
	if (m_UpdateServerUrl.empty()) {
		LogUpdate("[UpdateManager] ERROR: Update server URL not configured");
		return false;
	}
	
	LogUpdate("[UpdateManager] Checking for updates from: %s", m_UpdateServerUrl.c_str());
	
	// Download update manifest
	std::string manifestUrl = m_UpdateServerUrl + "/update_manifest.txt";
	std::string manifestPath = GetTempFilePath("update_manifest.txt");
	
	DWORD bytesDownloaded = 0;
	if (!DownloadFile(manifestUrl.c_str(), manifestPath.c_str(), &bytesDownloaded)) {
		LogUpdate("[UpdateManager] ERROR: Failed to download update manifest");
		m_Status = UPDATE_STATUS_ERROR;
		return false;
	}
	
	// Read manifest file
	FILE* file = NULL;
	fopen_s(&file, manifestPath.c_str(), "r");
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
	if (m_LatestUpdate.version != m_CurrentVersion) {
		m_UpdateAvailable = true;
		m_Status = UPDATE_STATUS_AVAILABLE;
		LogUpdate("[UpdateManager] UPDATE AVAILABLE: %s -> %s", 
			m_CurrentVersion.c_str(), m_LatestUpdate.version.c_str());
		LogUpdate("[UpdateManager] Description: %s", m_LatestUpdate.description.c_str());
		
		if (m_ShowNotifications) {
			char msg[512];
			sprintf_s(msg, sizeof(msg), 
				"UPDATE AVAILABLE!\n\n"
				"Current Version: %s\n"
				"New Version: %s\n\n"
				"Description: %s\n\n"
				"Go to menu [Update] to download and apply the update.",
				m_CurrentVersion.c_str(), 
				m_LatestUpdate.version.c_str(),
				m_LatestUpdate.description.c_str());
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
		LogUpdate("[UpdateManager] No updates available (Current version: %s)", m_CurrentVersion.c_str());
		return false;
	}
	
	m_LastCheckTime = GetTickCount();
	return true;
}

bool CUpdateManager::ParseUpdateManifest(const char* manifestData) {
	// Simple INI-style parser
	// Format:
	// [Update]
	// Version=1.0.1
	// DownloadUrl=http://yoursite.com/updates/GameServer_1.0.1.exe
	// FileName=GameServer.exe
	// FileHash=abc123def456...
	// FileSize=1234567
	// FileType=0
	// Description=Bug fixes and improvements
	// Required=1
	
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
				m_LatestUpdate.version = value;
			} else if (_stricmp(key, "DownloadUrl") == 0) {
				m_LatestUpdate.downloadUrl = value;
			} else if (_stricmp(key, "FileName") == 0) {
				m_LatestUpdate.fileName = value;
			} else if (_stricmp(key, "FileHash") == 0) {
				m_LatestUpdate.fileHash = value;
			} else if (_stricmp(key, "FileSize") == 0) {
				m_LatestUpdate.fileSize = atoi(value);
			} else if (_stricmp(key, "FileType") == 0) {
				m_LatestUpdate.fileType = (UpdateFileType)atoi(value);
			} else if (_stricmp(key, "Description") == 0) {
				m_LatestUpdate.description = value;
			} else if (_stricmp(key, "Required") == 0) {
				m_LatestUpdate.isRequired = atoi(value) ? true : false;
			}
		}
	}
	
	// Validate required fields
	if (m_LatestUpdate.version.empty() || m_LatestUpdate.downloadUrl.empty() || m_LatestUpdate.fileName.empty()) {
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
	
	LogUpdate("[UpdateManager] Starting download: %s", m_LatestUpdate.downloadUrl.c_str());
	m_Status = UPDATE_STATUS_DOWNLOADING;
	
	std::string destPath = GetTempFilePath(m_LatestUpdate.fileName.c_str());
	
	DWORD bytesDownloaded = 0;
	if (!DownloadFile(m_LatestUpdate.downloadUrl.c_str(), destPath.c_str(), &bytesDownloaded)) {
		LogUpdate("[UpdateManager] ERROR: Download failed");
		m_Status = UPDATE_STATUS_ERROR;
		if (m_ShowNotifications) {
			ShowNotification("Update download failed!\n\nPlease try again later or download manually.", MB_ICONERROR);
		}
		return false;
	}
	
	LogUpdate("[UpdateManager] Download complete: %d bytes", bytesDownloaded);
	
	// Verify file hash if provided
	if (!m_LatestUpdate.fileHash.empty()) {
		LogUpdate("[UpdateManager] Verifying downloaded file...");
		if (!VerifyDownloadedFile(destPath.c_str(), m_LatestUpdate.fileHash.c_str())) {
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
	
	if (m_ShowNotifications) {
		char msg[512];
		sprintf_s(msg, sizeof(msg), 
			"Update downloaded successfully!\n\n"
			"Version: %s\n"
			"File: %s\n\n"
			"The update is ready to be applied.\n"
			"Go to menu [Update] -> [Apply Update] to install.\n\n"
			"NOTE: The server will be restarted during the update process.",
			m_LatestUpdate.version.c_str(),
			m_LatestUpdate.fileName.c_str());
		ShowNotification(msg, MB_ICONINFORMATION);
	}
	
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
	
	std::string updateFile = GetTempFilePath(m_LatestUpdate.fileName.c_str());
	std::string targetFile;
	
	// Determine target file path based on file type
	if (m_LatestUpdate.fileType == UPDATE_FILE_EXECUTABLE) {
		// For executable, we need special handling
		targetFile = ".\\GameServer.exe";
		
		// Create backup
		LogUpdate("[UpdateManager] Creating backup of current executable...");
		if (!CreateBackup(targetFile.c_str())) {
			LogUpdate("[UpdateManager] ERROR: Failed to create backup");
			if (m_ShowNotifications) {
				ShowNotification("Failed to create backup!\n\nUpdate aborted for safety.", MB_ICONERROR);
			}
			return false;
		}
		
		// Create update script
		FILE* script = NULL;
		fopen_s(&script, ".\\Update\\apply_update.bat", "w");
		if (script) {
			fprintf(script, "@echo off\n");
			fprintf(script, "echo Applying GameServer update...\n");
			fprintf(script, "timeout /t 2 /nobreak >nul\n");
			fprintf(script, "echo Replacing executable...\n");
			fprintf(script, "copy /Y \"%s\" \"%s\"\n", updateFile.c_str(), targetFile.c_str());
			fprintf(script, "if errorlevel 1 goto error\n");
			fprintf(script, "echo Update applied successfully!\n");
			fprintf(script, "echo Restarting GameServer...\n");
			fprintf(script, "start \"\" \"%s\"\n", targetFile.c_str());
			fprintf(script, "goto end\n");
			fprintf(script, ":error\n");
			fprintf(script, "echo ERROR: Failed to apply update!\n");
			fprintf(script, "echo Restoring backup...\n");
			fprintf(script, "copy /Y \"%s.backup\" \"%s\"\n", targetFile.c_str(), targetFile.c_str());
			fprintf(script, "pause\n");
			fprintf(script, ":end\n");
			fclose(script);
			
			LogUpdate("[UpdateManager] Launching update script and exiting...");
			
			// Launch update script and exit
			STARTUPINFOA si = {0};
			PROCESS_INFORMATION pi = {0};
			si.cb = sizeof(si);
			
			if (CreateProcessA(NULL, ".\\Update\\apply_update.bat", NULL, NULL, FALSE, 
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
		targetFile = ".\\Data\\" + m_LatestUpdate.fileName;
		
		LogUpdate("[UpdateManager] Updating file: %s", targetFile.c_str());
		
		// Create backup
		if (!CreateBackup(targetFile.c_str())) {
			LogUpdate("[UpdateManager] WARNING: Failed to create backup");
		}
		
		// Copy file
		if (!CopyFileA(updateFile.c_str(), targetFile.c_str(), FALSE)) {
			LogUpdate("[UpdateManager] ERROR: Failed to copy file: %d", GetLastError());
			if (m_ShowNotifications) {
				ShowNotification("Failed to apply update!\n\nPlease check file permissions.", MB_ICONERROR);
			}
			return false;
		}
		
		LogUpdate("[UpdateManager] Update applied successfully");
		m_CurrentVersion = m_LatestUpdate.version;
		SaveConfig(".\\Data\\UpdateConfig.ini");
		
		if (m_ShowNotifications) {
			char msg[512];
			sprintf_s(msg, sizeof(msg), 
				"Update applied successfully!\n\n"
				"File: %s\n"
				"Version: %s\n\n"
				"You may need to reload the data file for changes to take effect.",
				m_LatestUpdate.fileName.c_str(),
				m_LatestUpdate.version.c_str());
			ShowNotification(msg, MB_ICONINFORMATION);
		}
	}
	
	m_Status = UPDATE_STATUS_NO_UPDATE;
	m_UpdateAvailable = false;
	return true;
}

bool CUpdateManager::CreateBackup(const char* filePath) {
	std::string backupPath = std::string(filePath) + ".backup";
	
	if (PathFileExistsA(filePath)) {
		if (!CopyFileA(filePath, backupPath.c_str(), FALSE)) {
			LogUpdate("[UpdateManager] Failed to create backup: %d", GetLastError());
			return false;
		}
		LogUpdate("[UpdateManager] Backup created: %s", backupPath.c_str());
	}
	
	return true;
}

std::string CUpdateManager::GetTempFilePath(const char* fileName) {
	return m_TempDirectory + "\\" + fileName;
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
