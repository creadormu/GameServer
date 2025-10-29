#pragma once

#include <string>
#include <vector>
#include <map>
#include <windows.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")

#define UPDATE_CHECK_INTERVAL 3600000  // 1 hour in milliseconds
#define UPDATE_TIMER_ID 999
#define IDD_UPDATE_DIALOG 3343

// Update status codes
enum UpdateStatus {
	UPDATE_STATUS_NO_UPDATE = 0,
	UPDATE_STATUS_AVAILABLE = 1,
	UPDATE_STATUS_DOWNLOADING = 2,
	UPDATE_STATUS_READY = 3,
	UPDATE_STATUS_ERROR = 4,
	UPDATE_STATUS_DISABLED = 5
};

// File types that can be updated
enum UpdateFileType {
	UPDATE_FILE_EXECUTABLE = 0,
	UPDATE_FILE_DATA = 1,
	UPDATE_FILE_CONFIG = 2,
	UPDATE_FILE_DLL = 3
};

// File update status
enum FileUpdateStatus {
	FILE_STATUS_PENDING = 0,
	FILE_STATUS_CHECKING = 1,
	FILE_STATUS_AVAILABLE = 2,
	FILE_STATUS_DOWNLOADING = 3,
	FILE_STATUS_DOWNLOADED = 4,
	FILE_STATUS_VERIFYING = 5,
	FILE_STATUS_READY = 6,
	FILE_STATUS_APPLIED = 7,
	FILE_STATUS_ERROR = 8,
	FILE_STATUS_UPTODATE = 9
};



// Update file information structure
struct UpdateFileInfo {
	std::string fileName;
	std::string version;
	std::string downloadUrl;
	std::string fileHash;
	DWORD fileSize;
	UpdateFileType fileType;
	std::string description;
	FileUpdateStatus status;
	bool isRequired;
	std::string targetPath;
	std::string currentVersion;

	UpdateFileInfo() : fileSize(0), fileType(UPDATE_FILE_DATA),
		status(FILE_STATUS_PENDING), isRequired(false) {
	}
};


// Update information structure
struct UpdateInfo {
	std::string version;
	std::string downloadUrl;
	std::string fileName;
	std::string fileHash;  // MD5 or SHA256 hash for verification
	DWORD fileSize;
	UpdateFileType fileType;
	std::string description;
	bool isRequired;       // If true, server must update
};

class CUpdateManager {
private:
	bool m_Enabled;
	bool m_AutoCheck;
	bool m_AutoDownload;
	bool m_ShowNotifications;
	std::string m_UpdateServerUrl;
	std::string m_CurrentVersion;
	std::string m_TempDirectory;
	
	UpdateStatus m_Status;
	UpdateInfo m_LatestUpdate;
	std::vector<UpdateFileInfo> m_AvailableUpdates;
	std::map<std::string, std::string> m_FileVersions;

	HWND m_hWnd;
	HANDLE m_DownloadThread;
	CRITICAL_SECTION m_CriticalSection;
	
	bool m_UpdateAvailable;
	DWORD m_LastCheckTime;

public:
	CUpdateManager();
	~CUpdateManager();

	// Initialization
	void Init(HWND hWnd);
	void LoadConfig(const char* configFile);
	void SaveConfig(const char* configFile);
	void LoadVersionManifest(const char* manifestFile);
	void SaveVersionManifest(const char* manifestFile);

	// Version management
	void SetFileVersion(const std::string& fileName, const std::string& version);
	std::string GetFileVersion(const std::string& fileName);
	int CompareVersions(const std::string& v1, const std::string& v2);


	// Update checking
	bool CheckForUpdates();
	bool IsUpdateAvailable();
	UpdateInfo GetUpdateInfo();
	std::vector<UpdateFileInfo> GetAvailableUpdates();

	// Download and install
	bool DownloadUpdate();
	bool DownloadFile(UpdateFileInfo& fileInfo);
	bool VerifyDownloadedFile(const char* filePath, const char* expectedHash);
	bool VerifyFile(const UpdateFileInfo& fileInfo);
	bool ApplyUpdate();
	bool ApplyUpdate(UpdateFileInfo& fileInfo);

	// Manual operations
	void ShowUpdateDialog();
	void ManualCheckForUpdates();
	
	// Automatic operations
	void EnableAutoUpdate(bool enable);
	void OnTimer();
	
	// Getters/Setters
	bool IsEnabled() const { return m_Enabled; }
	void SetEnabled(bool enabled) { m_Enabled = enabled; SaveConfig(".\\Data\\UpdateConfig.ini"); }

	bool IsAutoCheckEnabled() const { return m_AutoCheck; }
	void SetAutoCheck(bool enabled) { m_AutoCheck = enabled; SaveConfig(".\\Data\\UpdateConfig.ini"); }

	bool IsAutoDownloadEnabled() const { return m_AutoDownload; }
	void SetAutoDownload(bool enabled) { m_AutoDownload = enabled; SaveConfig(".\\Data\\UpdateConfig.ini"); }



	UpdateStatus GetStatus() const { return m_Status; }
	const char* GetStatusString() const;

	std::string GetCurrentVersion() const { return m_CurrentVersion; }
	void SetCurrentVersion(const char* version);

	std::string GetUpdateServerUrl() const { return m_UpdateServerUrl; }
	void SetUpdateServerUrl(const char* url) { m_UpdateServerUrl = url; }


	// Notification
	void ShowNotification(const char* message, UINT icon = MB_ICONINFORMATION);
	void LogUpdate(const char* format, ...);

	// Utility
	void SaveCurrentVersions();
	void RestartServer();

private:
	// Internal helpers
	bool DownloadFileInternal(const char* url, const char* destPath, DWORD* bytesDownloaded);
	bool ParseUpdateManifest(const char* manifestData);
	void ProcessUpdateFile(UpdateFileInfo& fileInfo);
	std::string CalculateFileHash(const char* filePath);
	bool CreateBackup(const char* filePath);
	bool RestoreBackup(const char* filePath);
	bool PrepareExecutableUpdate(const std::string& sourcePath, const std::string& targetPath);
	std::string GetTempFilePath(const char* fileName);

	// Thread functions
	static DWORD WINAPI DownloadThreadProc(LPVOID lpParam);
};


extern CUpdateManager gUpdateManager;
