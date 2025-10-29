#pragma once

#include <windows.h>
#include <commctrl.h>
#include <string>
#include <vector>

#pragma comment(lib, "comctl32.lib")

// Dialog control IDs
#define IDC_UPDATE_LIST 3326
#define IDC_BTN_CHECK 3327
#define IDC_BTN_DOWNLOAD 3328
#define IDC_BTN_APPLY 3329
#define IDC_BTN_CLOSE 3330
#define IDC_PROGRESS 3331
#define IDC_STATUS_TEXT 3332
#define IDC_VERSION_CURRENT 3333
#define IDC_VERSION_LATEST 3334
#define IDC_CHK_AUTO_UPDATE 3335
#define IDC_CHK_AUTO_DOWNLOAD 3336
#define IDC_BTN_VERIFY 3337
#define IDC_DESCRIPTION 3338
#define IDC_FILE_LIST 3339

// Update file status
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
};

class CUpdateDialog {
private:
    HWND m_hDlg;
    HWND m_hListView;
    HWND m_hProgress;
    HWND m_hStatusText;
    HWND m_hDescription;
    HWND m_hFileList;

    std::vector<UpdateFileInfo> m_UpdateFiles;
    bool m_IsChecking;
    bool m_IsDownloading;
    bool m_IsApplying;

    HFONT m_hTitleFont;
    HFONT m_hBoldFont;

public:
    CUpdateDialog();
    ~CUpdateDialog();

    INT_PTR Show(HWND hParent);
    void UpdateProgress(int current, int total, const char* status);
    void AddUpdateFile(const UpdateFileInfo& fileInfo);
    void ClearUpdateFiles();
    void RefreshList();
    void SetStatus(const char* status);

private:
    static INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

    void OnInitDialog(HWND hDlg);
    void OnCommand(WPARAM wParam);
    void OnCheckForUpdates();
    void OnDownloadUpdates();
    void OnApplyUpdates();
    void OnVerifyFiles();
    void OnClose();

    void InitializeListView();
    void UpdateListViewItem(int index);
    void EnableButtons(bool check, bool download, bool apply, bool verify);

    const char* GetStatusText(FileUpdateStatus status);
    COLORREF GetStatusColor(FileUpdateStatus status);
};

extern CUpdateDialog* g_UpdateDialog;