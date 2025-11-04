#ifndef _UPDATE_MANAGER_H_
#define _UPDATE_MANAGER_H_

#include <windows.h>
#include <wininet.h>


#pragma comment(lib, "wininet.lib")

#define MAX_UPDATES 32
#define UPDATE_CHECK_INTERVAL (60 * 60 * 1000)  // 1 hour

enum UpdateStatus {
    UPDATE_STATUS_NO_UPDATE,
    UPDATE_STATUS_AVAILABLE,
    UPDATE_STATUS_DOWNLOADING,
    UPDATE_STATUS_READY,
    UPDATE_STATUS_ERROR,
    UPDATE_STATUS_DISABLED
};

enum UpdateFileType {
    UPDATE_FILE_DATA = 0,
    UPDATE_FILE_EXECUTABLE = 1,
    UPDATE_FILE_LIBRARY = 2
};

struct UpdateInfo {
    char version[32];
    char downloadUrl[512];
    char fileName[256];
    char fileHash[128];
    DWORD fileSize;
    UpdateFileType fileType;
    char description[512];
    char targetPath[512];
    bool isRequired;

    UpdateInfo() {
        memset(this, 0, sizeof(UpdateInfo));
    }
};

class CUpdateManager {
public:
    CUpdateManager();
    ~CUpdateManager();

    void Init(HWND hWnd);
    void LoadConfig(const char* configFile);
    void SaveConfig(const char* configFile);

    bool CheckForUpdates();
    bool DownloadUpdate();  // Keep for backwards compatibility
    bool DownloadAllUpdates();  // NEW
    bool ApplyUpdate();     // Keep for backwards compatibility
    bool ApplyAllUpdates(); // NEW

    void ManualCheckForUpdates();
    void EnableAutoUpdate(bool enable);
    void ShowConfigDialog();

    void OnTimer();

    // Getters
    bool IsEnabled() const { return m_Enabled; }
    bool IsUpdateAvailable();
    UpdateInfo GetUpdateInfo();
    UpdateStatus GetStatus() const { return m_Status; }
    const char* GetStatusString() const;
    const char* GetCurrentVersion() const { return m_CurrentVersion; }
    const char* GetUpdateServerUrl() const { return m_UpdateServerUrl; }
    int GetDownloadProgress() const { return m_DownloadProgress; }
    int GetUpdateCount() const { return m_UpdateCount; }  // NEW

    // Setters
    void SetCurrentVersion(const char* version);
    void SetUpdateServerUrl(const char* url);
    void SetDownloadProgress(int percent);

private:
    bool ParseUpdateManifest(const char* manifestData);
    bool DownloadFile(const char* url, const char* destPath, DWORD* bytesDownloaded);
    bool DownloadFileWithProgress(const char* url, const char* destPath, DWORD* bytesDownloaded);
    bool VerifyDownloadedFile(const char* filePath, const char* expectedHash);
    bool CreateBackup(const char* filePath);
    void GetTempFilePath(const char* fileName, char* outPath, int outPathSize);
    void CalculateFileHash(const char* filePath, char* outHash, int outHashSize);
    void ShowNotification(const char* message, UINT icon);
    void LogUpdate(const char* format, ...);
    void CleanupTempFiles();
    bool DeleteDirectoryRecursive(const char* dirPath);
    bool IsVersionNewer(const char* version1, const char* version2);
    bool IsUpdateAlreadyInstalled(const char* fileName, const char* version);
    void SaveInstalledUpdate(const char* fileName, const char* version);

    void ShowProgressDialog(const char* fileName);
    void UpdateProgressDialog(int percent, DWORD current, DWORD total);
    void CloseProgressDialog();

    void ShowUpdateSummary();  // NEW
    void CreateDirectoryStructure(const char* filePath);  // NEW

private:
    bool m_Enabled;
    bool m_AutoCheck;
    bool m_AutoDownload;
    bool m_ShowNotifications;
    char m_UpdateServerUrl[512];
    char m_CurrentVersion[32];
    char m_TempDirectory[256];

    UpdateStatus m_Status;
    bool m_UpdateAvailable;
    DWORD m_LastCheckTime;
    int m_DownloadProgress;

    UpdateInfo m_AvailableUpdates[MAX_UPDATES];  // CHANGED from m_LatestUpdate
    int m_UpdateCount;  // NEW

    HWND m_hWnd;
    HWND m_hProgressWnd;
    HANDLE m_DownloadThread;
    CRITICAL_SECTION m_CriticalSection;
};

extern CUpdateManager gUpdateManager;

// Dialog procedures
INT_PTR CALLBACK UpdateConfigDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ProgressDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

#endif // _UPDATE_MANAGER_H_