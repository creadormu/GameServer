#pragma once

#include <string>
#include <windows.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")

#define UPDATE_CHECK_INTERVAL 3600000  // 1 hour in milliseconds
#define UPDATE_TIMER_ID 999

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
	UPDATE_FILE_EXECUTABLE = 0,  // GameServer.exe
	UPDATE_FILE_DATA = 1,        // Data files like Item.txt
	UPDATE_FILE_CONFIG = 2       // Configuration files
};

// Update information structure
struct UpdateInfo {
	char version[32];
	char downloadUrl[512];
	char fileName[256];
	char fileHash[128];
	DWORD fileSize;
	UpdateFileType fileType;
	char description[256];
	bool isRequired;
	
	UpdateInfo() {
		memset(version, 0, sizeof(version));
		memset(downloadUrl, 0, sizeof(downloadUrl));
		memset(fileName, 0, sizeof(fileName));
		memset(fileHash, 0, sizeof(fileHash));
		fileSize = 0;
		fileType = UPDATE_FILE_EXECUTABLE;
		memset(description, 0, sizeof(description));
		isRequired = false;
	}
};

class CUpdateManager {
private:
	bool m_Enabled;
	bool m_AutoCheck;
	bool m_AutoDownload;
	bool m_ShowNotifications;
	char m_UpdateServerUrl[512];
	char m_CurrentVersion[32];
	char m_TempDirectory[256];
	
	UpdateStatus m_Status;
	UpdateInfo m_LatestUpdate;
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

	// Update checking
	bool CheckForUpdates();
	bool IsUpdateAvailable();
	UpdateInfo GetUpdateInfo();
	
	// Download and install
	bool DownloadUpdate();
	bool VerifyDownloadedFile(const char* filePath, const char* expectedHash);
	bool ApplyUpdate();
	
	// Manual operations
	void ShowUpdateDialog();
	void ManualCheckForUpdates();
	
	// Automatic operations
	void EnableAutoUpdate(bool enable);
	void OnTimer();
	
	// Getters/Setters
	bool IsEnabled() const { return m_Enabled; }
	void SetEnabled(bool enabled) { m_Enabled = enabled; }
	
	UpdateStatus GetStatus() const { return m_Status; }
	const char* GetStatusString() const;
	
	const char* GetCurrentVersion() const { return m_CurrentVersion; }
	void SetCurrentVersion(const char* version);
	
	const char* GetUpdateServerUrl() const { return m_UpdateServerUrl; }
	void SetUpdateServerUrl(const char* url);

	// Notification
	void ShowNotification(const char* message, UINT icon = MB_ICONINFORMATION);
	void LogUpdate(const char* format, ...);

private:
	// Internal helpers
	bool DownloadFile(const char* url, const char* destPath, DWORD* bytesDownloaded);
	bool ParseUpdateManifest(const char* manifestData);
	void CalculateFileHash(const char* filePath, char* outHash, int outHashSize);
	bool CreateBackup(const char* filePath);
	bool RestoreBackup(const char* filePath);
	void GetTempFilePath(const char* fileName, char* outPath, int outPathSize);
	
	// Thread functions
	static DWORD WINAPI DownloadThreadProc(LPVOID lpParam);
};

extern CUpdateManager gUpdateManager;
