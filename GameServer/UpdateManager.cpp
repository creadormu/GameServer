#include "stdafx.h"
#include "UpdateManager.h"
#include "UpdateDialog.h"
#include "Log.h"
#include "Util.h"
#include <shlwapi.h>
#include <fstream>
#include <sstream>
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

	// Create directories
	CreateDirectoryA(".\\Update", NULL);
	CreateDirectoryA(".\\Update\\Backup", NULL);
	CreateDirectoryA(m_TempDirectory.c_str(), NULL);

	// Load configuration
	LoadConfig(".\\Data\\UpdateConfig.ini");

	// Load file versions from version manifest
	LoadVersionManifest(".\\Update\\versions.txt");

	LogUpdate("[UpdateManager] Initialized");
	LogUpdate("[UpdateManager] Server Version: %s", m_CurrentVersion.c_str());
	LogUpdate("[UpdateManager] Auto-Update: %s", m_Enabled ? "ENABLED" : "DISABLED");
	LogUpdate("[UpdateManager] Tracked Files: %d", (int)m_FileVersions.size());

	if (m_Enabled && m_UpdateServerUrl.empty()) {
		LogUpdate("[UpdateManager] WARNING: No update server URL configured!");
		m_Enabled = false;
	}

	// Initial check if enabled
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

void CUpdateManager::LoadVersionManifest(const char* manifestFile) {
	m_FileVersions.clear();

	std::ifstream file(manifestFile);
	if (!file.is_open()) {
		LogUpdate("[UpdateManager] No version manifest found, creating new one...");
		SaveVersionManifest(manifestFile);
		return;
	}

	std::string line;
	while (std::getline(file, line)) {
		if (line.empty() || line[0] == '#' || line[0] == ';') continue;

		size_t pos = line.find('=');
		if (pos != std::string::npos) {
			std::string fileName = line.substr(0, pos);
			std::string version = line.substr(pos + 1);

			// Trim whitespace
			fileName.erase(0, fileName.find_first_not_of(" \t\r\n"));
			fileName.erase(fileName.find_last_not_of(" \t\r\n") + 1);
			version.erase(0, version.find_first_not_of(" \t\r\n"));
			version.erase(version.find_last_not_of(" \t\r\n") + 1);

			m_FileVersions[fileName] = version;
			LogUpdate("[UpdateManager] Tracked: %s = %s", fileName.c_str(), version.c_str());
		}
	}
	file.close();
}

void CUpdateManager::SaveVersionManifest(const char* manifestFile) {
	std::ofstream file(manifestFile);
	if (!file.is_open()) {
		LogUpdate("[UpdateManager] ERROR: Cannot save version manifest");
		return;
	}

	file << "# MuServer Version Manifest\n";
	file << "# Format: FileName=Version\n";
	file << "# Generated: " << __DATE__ << " " << __TIME__ << "\n\n";

	file << "GameServer.exe=" << m_CurrentVersion << "\n";

	for (const auto& pair : m_FileVersions) {
		file << pair.first << "=" << pair.second << "\n";
	}

	file.close();
	LogUpdate("[UpdateManager] Version manifest saved");
}

void CUpdateManager::SetFileVersion(const std::string& fileName, const std::string& version) {
	m_FileVersions[fileName] = version;
	SaveVersionManifest(".\\Update\\versions.txt");
	LogUpdate("[UpdateManager] Updated version: %s = %s", fileName.c_str(), version.c_str());
}

std::string CUpdateManager::GetFileVersion(const std::string& fileName) {
	auto

		auto it = m_FileVersions.find(fileName);
	if (it != m_FileVersions.end()) {
		return it->second;
	}
	return "0.0.0";
}

bool CUpdateManager::CheckForUpdates() {
	if (!m_Enabled) {
		LogUpdate("[UpdateManager] Update check skipped - disabled");
		return false;
	}

	if (m_UpdateServerUrl.empty()) {
		LogUpdate("[UpdateManager] ERROR: No update server URL");
		return false;
	}

	LogUpdate("[UpdateManager] Checking for updates: %s", m_UpdateServerUrl.c_str());

	// Download master manifest
	std::string manifestUrl = m_UpdateServerUrl + "/update_manifest.txt";
	std::string manifestPath = GetTempFilePath("update_manifest.txt");

	DWORD bytesDownloaded = 0;
	if (!DownloadFileInternal(manifestUrl.c_str(), manifestPath.c_str(), &bytesDownloaded)) {
		LogUpdate("[UpdateManager] ERROR: Failed to download manifest");
		m_Status = UPDATE_STATUS_ERROR;
		return false;
	}

	// Read manifest
	std::ifstream file(manifestPath);
	if (!file.is_open()) {
		LogUpdate("[UpdateManager] ERROR: Cannot open manifest");
		m_Status = UPDATE_STATUS_ERROR;
		return false;
	}

	m_AvailableUpdates.clear();
	std::string line;
	UpdateFileInfo currentFile;
	bool inSection = false;

	while (std::getline(file, line)) {
		// Trim
		line.erase(0, line.find_first_not_of(" \t\r\n"));
		line.erase(line.find_last_not_of(" \t\r\n") + 1);

		if (line.empty() || line[0] == '#' || line[0] == ';') continue;

		if (line[0] == '[') {
			// Save previous file if exists
			if (inSection) {
				ProcessUpdateFile(currentFile);
			}
			// Start new section
			currentFile = UpdateFileInfo();
			inSection = true;
			continue;
		}

		size_t pos = line.find('=');
		if (pos != std::string::npos) {
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 1);

			// Trim
			key.erase(key.find_last_not_of(" \t") + 1);
			value.erase(0, value.find_first_not_of(" \t"));

			if (_stricmp(key.c_str(), "FileName") == 0) {
				currentFile.fileName = value;
			}
			else if (_stricmp(key.c_str(), "Version") == 0) {
				currentFile.version = value;
			}
			else if (_stricmp(key.c_str(), "DownloadUrl") == 0) {
				currentFile.downloadUrl = value;
			}
			else if (_stricmp(key.c_str(), "FileHash") == 0) {
				currentFile.fileHash = value;
			}
			else if (_stricmp(key.c_str(), "FileSize") == 0) {
				currentFile.fileSize = atoi(value.c_str());
			}
			else if (_stricmp(key.c_str(), "FileType") == 0) {
				currentFile.fileType = (UpdateFileType)atoi(value.c_str());
			}
			else if (_stricmp(key.c_str(), "Description") == 0) {
				currentFile.description = value;
			}
			else if (_stricmp(key.c_str(), "Required") == 0) {
				currentFile.isRequired = atoi(value.c_str()) != 0;
			}
			else if (_stricmp(key.c_str(), "TargetPath") == 0) {
				currentFile.targetPath = value;
			}
		}
	}

	// Process last file
	if (inSection) {
		ProcessUpdateFile(currentFile);
	}

	file.close();

	// Update status
	m_UpdateAvailable = !m_AvailableUpdates.empty();
	m_Status = m_UpdateAvailable ? UPDATE_STATUS_AVAILABLE : UPDATE_STATUS_NO_UPDATE;
	m_LastCheckTime = GetTickCount();

	LogUpdate("[UpdateManager] Check complete: %d update(s) available", (int)m_AvailableUpdates.size());

	return true;
}

void CUpdateManager::ProcessUpdateFile(UpdateFileInfo& fileInfo) {
	if (fileInfo.fileName.empty() || fileInfo.version.empty()) {
		return;
	}

	// Get current version
	fileInfo.currentVersion = GetFileVersion(fileInfo.fileName);

	// Set default target path if not specified
	if (fileInfo.targetPath.empty()) {
		if (fileInfo.fileType == UPDATE_FILE_EXECUTABLE) {
			fileInfo.targetPath = ".\\" + fileInfo.fileName;
		}
		else {
			fileInfo.targetPath = ".\\Data\\" + fileInfo.fileName;
		}
	}

	// Compare versions
	if (CompareVersions(fileInfo.version, fileInfo.currentVersion) > 0) {
		fileInfo.status = FILE_STATUS_AVAILABLE;
		m_AvailableUpdates.push_back(fileInfo);
		LogUpdate("[UpdateManager] Update available: %s (%s -> %s)",
			fileInfo.fileName.c_str(),
			fileInfo.currentVersion.c_str(),
			fileInfo.version.c_str());
	}
}

int CUpdateManager::CompareVersions(const std::string& v1, const std::string& v2) {
	// Parse version strings (e.g., "1.2.3")
	int major1 = 0, minor1 = 0, patch1 = 0;
	int major2 = 0, minor2 = 0, patch2 = 0;

	sscanf_s(v1.c_str(), "%d.%d.%d", &major1, &minor1, &patch1);
	sscanf_s(v2.c_str(), "%d.%d.%d", &major2, &minor2, &patch2);

	if (major1 != major2) return major1 - major2;
	if (minor1 != minor2) return minor1 - minor2;
	return patch1 - patch2;
}

std::vector<UpdateFileInfo> CUpdateManager::GetAvailableUpdates() {
	return m_AvailableUpdates;
}

bool CUpdateManager::DownloadFile(UpdateFileInfo& fileInfo) {
	if (fileInfo.downloadUrl.empty()) {
		LogUpdate("[UpdateManager] ERROR: No download URL for %s", fileInfo.fileName.c_str());
		return false;
	}

	std::string destPath = GetTempFilePath(fileInfo.fileName.c_str());
	LogUpdate("[UpdateManager] Downloading: %s", fileInfo.fileName.c_str());

	DWORD bytesDownloaded = 0;
	if (!DownloadFileInternal(fileInfo.downloadUrl.c_str(), destPath.c_str(), &bytesDownloaded)) {
		LogUpdate("[UpdateManager] ERROR: Download failed for %s", fileInfo.fileName.c_str());
		return false;
	}

	LogUpdate("[UpdateManager] Downloaded: %s (%d bytes)", fileInfo.fileName.c_str(), bytesDownloaded);
	return true;
}

bool CUpdateManager::DownloadFileInternal(const char* url, const char* destPath, DWORD* bytesDownloaded) {
	HINTERNET hInternet = InternetOpenA("MuServer UpdateManager/2.0",
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
		LogUpdate("[UpdateManager] ERROR: Cannot create file: %s", destPath);
		InternetCloseHandle(hUrl);
		InternetCloseHandle(hInternet);
		return false;
	}

	BYTE buffer[8192];
	DWORD totalBytes = 0;
	DWORD dwRead = 0;

	while (InternetReadFile(hUrl, buffer, sizeof(buffer), &dwRead) && dwRead > 0) {
		fwrite(buffer, 1, dwRead, file);
		totalBytes += dwRead;
	}

	fclose(file);
	InternetCloseHandle(hUrl);
	InternetCloseHandle(hInternet);

	if (bytesDownloaded) {
		*bytesDownloaded = totalBytes;
	}

	return totalBytes > 0;
}

bool CUpdateManager::VerifyFile(const UpdateFileInfo& fileInfo) {
	std::string filePath = GetTempFilePath(fileInfo.fileName.c_str());

	// Check file exists
	WIN32_FILE_ATTRIBUTE_DATA fileData;
	if (!GetFileAttributesExA(filePath.c_str(), GetFileExInfoStandard, &fileData)) {
		LogUpdate("[UpdateManager] ERROR: File not found: %s", filePath.c_str());
		return false;
	}

	DWORD fileSize = fileData.nFileSizeLow;

	// Verify size
	if (fileInfo.fileSize > 0 && fileSize != fileInfo.fileSize) {
		LogUpdate("[UpdateManager] ERROR: Size mismatch for %s (expected %d, got %d)",
			fileInfo.fileName.c_str(), fileInfo.fileSize, fileSize);
		return false;
	}

	// TODO: Implement hash verification if needed
	if (!fileInfo.fileHash.empty()) {
		// Add MD5/SHA256 verification here
	}

	LogUpdate("[UpdateManager] Verification OK: %s", fileInfo.fileName.c_str());
	return true;
}

bool CUpdateManager::ApplyUpdate(UpdateFileInfo& fileInfo) {
	std::string sourcePath = GetTempFilePath(fileInfo.fileName.c_str());
	std::string targetPath = fileInfo.targetPath;

	LogUpdate("[UpdateManager] Applying: %s -> %s", fileInfo.fileName.c_str(), targetPath.c_str());

	// Create backup
	if (!CreateBackup(targetPath.c_str())) {
		LogUpdate("[UpdateManager] WARNING: Backup failed for %s", targetPath.c_str());
	}

	// Special handling for executable
	if (fileInfo.fileType == UPDATE_FILE_EXECUTABLE) {
		return PrepareExecutableUpdate(sourcePath, targetPath);
	}

	// Regular file update
	if (!CopyFileA(sourcePath.c_str(), targetPath.c_str(), FALSE)) {
		LogUpdate("[UpdateManager] ERROR: Copy failed: %d", GetLastError());
		return false;
	}

	// Update version
	SetFileVersion(fileInfo.fileName, fileInfo.version);

	LogUpdate("[UpdateManager] Successfully applied: %s", fileInfo.fileName.c_str());
	return true;
}

bool CUpdateManager::PrepareExecutableUpdate(const std::string& sourcePath, const std::string& targetPath) {
	// Create updater batch script
	std::string scriptPath = ".\\Update\\apply_update.bat";
	std::ofstream script(scriptPath);

	if (!script.is_open()) {
		LogUpdate("[UpdateManager] ERROR: Cannot create update script");
		return false;
	}

	script << "@echo off\n";
	script << "title MuServer Update - Please Wait\n";
	script << "echo.\n";
	script << "echo ========================================\n";
	script << "echo    MuServer Update in Progress\n";
	script << "echo ========================================\n";
	script << "echo.\n";
	script << "echo Waiting for server to close...\n";
	script << "timeout /t 3 /nobreak >nul\n";
	script << "echo.\n";
	script << "echo Applying update...\n";
	script << "copy /Y \"" << sourcePath << "\" \"" << targetPath << "\"\n";
	script << "if errorlevel 1 (\n";
	script << "    echo ERROR: Update failed!\n";
	script << "    echo Restoring backup...\n";
	script << "    copy /Y \"" << targetPath << ".backup\" \"" << targetPath << "\"\n";
	script << "    pause\n";
	script << "    exit /b 1\n";
	script << ")\n";
	script << "echo.\n";
	script << "echo Update successful!\n";
	script << "echo Restarting server...\n";
	script << "timeout /t 2 /nobreak >nul\n";
	script << "start \"MuServer\" \"" << targetPath << "\"\n";
	script << "exit\n";

	script.close();

	// Launch updater
	STARTUPINFOA si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	si.cb = sizeof(si);

	char cmdLine[512];
	sprintf_s(cmdLine, "cmd.exe /c \"%s\"", scriptPath.c_str());

	if (!CreateProcessA(NULL, cmdLine, NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
		LogUpdate("[UpdateManager] ERROR: Cannot launch updater");
		return false;
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	LogUpdate("[UpdateManager] Updater launched, exiting server...");

	// Exit server
	PostQuitMessage(0);
	return true;
}

bool CUpdateManager::CreateBackup(const char* filePath) {
	if (!PathFileExistsA(filePath)) {
		return true; // Nothing to backup
	}

	// Create backup directory
	CreateDirectoryA(".\\Update\\Backup", NULL);

	// Generate backup filename with timestamp
	SYSTEMTIME st;
	GetLocalTime(&st);

	char backupPath[MAX_PATH];
	sprintf_s(backupPath, ".\\Update\\Backup\\%s_%04d%02d%02d_%02d%02d%02d.backup",
		PathFindFileNameA(filePath),
		st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond);

	if (!CopyFileA(filePath, backupPath, FALSE)) {
		LogUpdate("[UpdateManager] Backup failed: %d", GetLastError());
		return false;
	}

	LogUpdate("[UpdateManager] Backup created: %s", backupPath);
	return true;
}

void CUpdateManager::SaveCurrentVersions() {
	SaveVersionManifest(".\\Update\\versions.txt");
	SaveConfig(".\\Data\\UpdateConfig.ini");
}

void CUpdateManager::RestartServer() {
	// This is called after non-executable updates
	// Server can reload configs without full restart
	LogUpdate("[UpdateManager] Reloading configuration...");

	// TODO: Add your reload functions here
	// ReloadItemList();
	// ReloadMonsterData();
	// etc.
}

void CUpdateManager::ShowUpdateDialog() {
	if (!g_UpdateDialog) {
		g_UpdateDialog = new CUpdateDialog();
	}
	g_UpdateDialog->Show(m_hWnd);
}

std::string CUpdateManager::GetTempFilePath(const char* fileName) {
	return m_TempDirectory + "\\" + fileName;
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
	case UPDATE_STATUS_AVAILABLE: return "Updates available";
	case UPDATE_STATUS_DOWNLOADING: return "Downloading...";
	case UPDATE_STATUS_READY: return "Ready to install";
	case UPDATE_STATUS_ERROR: return "Error";
	case UPDATE_STATUS_DISABLED: return "Disabled";
	default: return "Unknown";
	}
}