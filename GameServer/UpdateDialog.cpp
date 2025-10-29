#include "stdafx.h"
#include "UpdateDialog.h"
#include "UpdateManager.h"
#include "resource.h"
#include <CommCtrl.h>

CUpdateDialog* g_UpdateDialog = nullptr;

CUpdateDialog::CUpdateDialog() {
    m_hDlg = NULL;
    m_hListView = NULL;
    m_hProgress = NULL;
    m_hStatusText = NULL;
    m_hDescription = NULL;
    m_hFileList = NULL;
    m_IsChecking = false;
    m_IsDownloading = false;
    m_IsApplying = false;
    m_hTitleFont = NULL;
    m_hBoldFont = NULL;
}

CUpdateDialog::~CUpdateDialog() {
    if (m_hTitleFont) DeleteObject(m_hTitleFont);
    if (m_hBoldFont) DeleteObject(m_hBoldFont);
}

INT_PTR CUpdateDialog::Show(HWND hParent) {
    return DialogBoxParam(GetModuleHandle(NULL), 
        MAKEINTRESOURCE(IDD_UPDATE_DIALOG), 
        hParent, DialogProc, (LPARAM)this);
}

INT_PTR CALLBACK CUpdateDialog::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    CUpdateDialog* pThis = (CUpdateDialog*)GetWindowLongPtr(hDlg, GWLP_USERDATA);
    
    switch (message) {
        case WM_INITDIALOG:
            SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);
            pThis = (CUpdateDialog*)lParam;
            pThis->m_hDlg = hDlg;
            pThis->OnInitDialog(hDlg);
            return TRUE;
            
        case WM_COMMAND:
            if (pThis) pThis->OnCommand(wParam);
            return TRUE;
            
        case WM_CLOSE:
            if (pThis) pThis->OnClose();
            return TRUE;
            
        case WM_CTLCOLORSTATIC: {
            HDC hdcStatic = (HDC)wParam;
            HWND hStatic = (HWND)lParam;
            
            if (pThis && hStatic == pThis->m_hStatusText) {
                SetBkMode(hdcStatic, TRANSPARENT);
                return (INT_PTR)GetStockObject(NULL_BRUSH);
            }
            break;
        }
    }
    
    return FALSE;
}

void CUpdateDialog::OnInitDialog(HWND hDlg) {
    // Center dialog
    RECT rcParent, rcDlg;
    GetWindowRect(GetParent(hDlg), &rcParent);
    GetWindowRect(hDlg, &rcDlg);
    
    int x = rcParent.left + (rcParent.right - rcParent.left - (rcDlg.right - rcDlg.left)) / 2;
    int y = rcParent.top + (rcParent.bottom - rcParent.top - (rcDlg.bottom - rcDlg.top)) / 2;
    SetWindowPos(hDlg, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    
    // Create fonts
    LOGFONT lf;
    GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);
    lf.lfHeight = -16;
    lf.lfWeight = FW_BOLD;
    m_hTitleFont = CreateFontIndirect(&lf);
    
    lf.lfHeight = -12;
    m_hBoldFont = CreateFontIndirect(&lf);
    
    // Get control handles
    m_hProgress = GetDlgItem(hDlg, IDC_PROGRESS);
    m_hStatusText = GetDlgItem(hDlg, IDC_STATUS_TEXT);
    m_hDescription = GetDlgItem(hDlg, IDC_DESCRIPTION);
    
    // Set title font
    SendDlgItemMessage(hDlg, IDC_STATIC, WM_SETFONT, (WPARAM)m_hTitleFont, TRUE);
    
    // Initialize ListView
    InitializeListView();
    
    // Set version labels
    char versionText[128];
    sprintf_s(versionText, "Current: %s", gUpdateManager.GetCurrentVersion().c_str());
    SetDlgItemTextA(hDlg, IDC_VERSION_CURRENT, versionText);
    SetDlgItemTextA(hDlg, IDC_VERSION_LATEST, "Latest: Checking...");
    
    // Set checkboxes
    CheckDlgButton(hDlg, IDC_CHK_AUTO_UPDATE, gUpdateManager.IsAutoCheckEnabled() ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_CHK_AUTO_DOWNLOAD, gUpdateManager.IsAutoDownloadEnabled() ? BST_CHECKED : BST_UNCHECKED);
    
    // Set initial status
    SetStatus("Ready. Click 'Check for Updates' to begin.");
    
    // Enable/disable buttons
    EnableButtons(true, false, false, false);
}

void CUpdateDialog::InitializeListView() {
    m_hListView = GetDlgItem(m_hDlg, IDC_UPDATE_LIST);
    
    // Set extended styles
    ListView_SetExtendedListViewStyle(m_hListView, 
        LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
    
    // Add columns
    LVCOLUMN lvc = {0};
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    
    lvc.pszText = "File Name";
    lvc.cx = 180;
    ListView_InsertColumn(m_hListView, 0, &lvc);
    
    lvc.pszText = "Current";
    lvc.cx = 80;
    ListView_InsertColumn(m_hListView, 1, &lvc);
    
    lvc.pszText = "Latest";
    lvc.cx = 80;
    ListView_InsertColumn(m_hListView, 2, &lvc);
    
    lvc.pszText = "Size";
    lvc.cx = 80;
    ListView_InsertColumn(m_hListView, 3, &lvc);
    
    lvc.pszText = "Status";
    lvc.cx = 120;
    ListView_InsertColumn(m_hListView, 4, &lvc);
}

void CUpdateDialog::OnCommand(WPARAM wParam) {
    int id = LOWORD(wParam);
    
    switch (id) {
        case IDC_BTN_CHECK:
            OnCheckForUpdates();
            break;
            
        case IDC_BTN_DOWNLOAD:
            OnDownloadUpdates();
            break;
            
        case IDC_BTN_APPLY:
            OnApplyUpdates();
            break;
            
        case IDC_BTN_VERIFY:
            OnVerifyFiles();
            break;
            
        case IDC_BTN_CLOSE:
        case IDCANCEL:
            OnClose();
            break;
            
        case IDC_CHK_AUTO_UPDATE:
            gUpdateManager.SetAutoCheck(IsDlgButtonChecked(m_hDlg, IDC_CHK_AUTO_UPDATE) == BST_CHECKED);
            break;
            
        case IDC_CHK_AUTO_DOWNLOAD:
            gUpdateManager.SetAutoDownload(IsDlgButtonChecked(m_hDlg, IDC_CHK_AUTO_DOWNLOAD) == BST_CHECKED);
            break;
    }
}

void CUpdateDialog::OnCheckForUpdates() {
    SetStatus("Checking for updates...");
    EnableButtons(false, false, false, false);
    m_IsChecking = true;
    
    ClearUpdateFiles();
    
    // Check synchronously for simplicity
    if (gUpdateManager.CheckForUpdates()) {
        auto updates = gUpdateManager.GetAvailableUpdates();
        
        for (const auto& update : updates) {
            AddUpdateFile(update);
        }
        
        if (updates.empty()) {
            SetStatus("All files are up to date!");
            EnableButtons(true, false, false, false);
        } else {
            char status[128];
            sprintf_s(status, "Found %d file(s) to update", (int)updates.size());
            SetStatus(status);
            EnableButtons(true, true, false, true);
        }
    } else {
        SetStatus("Failed to check for updates. Please verify your connection.");
        EnableButtons(true, false, false, false);
    }
    
    m_IsChecking = false;
    RefreshList();
}

void CUpdateDialog::OnDownloadUpdates() {
    SetStatus("Downloading updates...");
    EnableButtons(false, false, false, false);
    m_IsDownloading = true;
    
    SendMessage(m_hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, m_UpdateFiles.size()));
    SendMessage(m_hProgress, PBM_SETPOS, 0, 0);
    
    int total = m_UpdateFiles.size();
    int current = 0;
    bool allReady = true;
    
    for (auto& file : m_UpdateFiles) {
        if (file.status == FILE_STATUS_AVAILABLE || file.status == FILE_STATUS_ERROR) {
            file.status = FILE_STATUS_DOWNLOADING;
            RefreshList();
            
            char status[256];
            sprintf_s(status, "Downloading %s (%d/%d)...", file.fileName.c_str(), current + 1, total);
            SetStatus(status);
            
            if (gUpdateManager.DownloadFile(file)) {
                file.status = FILE_STATUS_VERIFYING;
                RefreshList();
                
                if (gUpdateManager.VerifyFile(file)) {
                    file.status = FILE_STATUS_READY;
                } else {
                    file.status = FILE_STATUS_ERROR;
                    allReady = false;
                }
            } else {
                file.status = FILE_STATUS_ERROR;
                allReady = false;
            }
            
            RefreshList();
        }
        
        current++;
        SendMessage(m_hProgress, PBM_SETPOS, current, 0);
    }
    
    if (allReady) {
        SetStatus("All updates downloaded and verified! Ready to apply.");
        EnableButtons(true, false, true, true);
    } else {
        SetStatus("Some downloads failed. Please retry or check logs.");
        EnableButtons(true, true, false, true);
    }
    
    m_IsDownloading = false;
    SendMessage(m_hProgress, PBM_SETPOS, 0, 0);
}

void CUpdateDialog::OnApplyUpdates() {
    int result = MessageBox(m_hDlg,
        "Are you sure you want to apply these updates?\n\n"
        "• Backups will be created automatically\n"
        "• Server will restart if executable is updated\n"
        "• Make sure all players are disconnected\n\n"
        "Continue?",
        "Confirm Update",
        MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);
    
    if (result != IDYES) return;
    
    SetStatus("Applying updates...");
    EnableButtons(false, false, false, false);
    m_IsApplying = true;
    
    SendMessage(m_hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, m_UpdateFiles.size()));
    SendMessage(m_hProgress, PBM_SETPOS, 0, 0);
    
    int total = m_UpdateFiles.size();
    int current = 0;
    bool hasExecutable = false;
    
    for (auto& file : m_UpdateFiles) {
        if (file.status == FILE_STATUS_READY) {
            char status[256];
            sprintf_s(status, "Applying %s (%d/%d)...", file.fileName.c_str(), current + 1, total);
            SetStatus(status);
            
            if (gUpdateManager.ApplyUpdate(file)) {
                file.status = FILE_STATUS_APPLIED;
                if (file.fileType == UPDATE_FILE_EXECUTABLE) {
                    hasExecutable = true;
                }
            } else {
                file.status = FILE_STATUS_ERROR;
            }
            
            RefreshList();
        }
        
        current++;
        SendMessage(m_hProgress, PBM_SETPOS, current, 0);
    }
    
    if (hasExecutable) {
        SetStatus("Updates applied! Server will restart in 5 seconds...");
        Sleep(5000);
        gUpdateManager.RestartServer();
    } else {
        SetStatus("All updates applied successfully!");
        gUpdateManager.SaveCurrentVersions();
    }
    
    m_IsApplying = false;
    EnableButtons(true, false, false, true);
    SendMessage(m_hProgress, PBM_SETPOS, 0, 0);
}

void CUpdateDialog::OnVerifyFiles() {
    SetStatus("Verifying downloaded files...");
    
    int verified = 0;
    int failed = 0;
    
    for (auto& file : m_UpdateFiles) {
        if (file.status == FILE_STATUS_READY || file.status == FILE_STATUS_ERROR) {
            if (gUpdateManager.VerifyFile(file)) {
                file.status = FILE_STATUS_READY;
                verified++;
            } else {
                file.status = FILE_STATUS_ERROR;
                failed++;
            }
        }
    }
    
    RefreshList();
    
    char status[128];
    sprintf_s(status, "Verification complete: %d OK, %d Failed", verified, failed);
    SetStatus(status);
}

void CUpdateDialog::OnClose() {
    if (m_IsChecking || m_IsDownloading || m_IsApplying) {
        MessageBox(m_hDlg, 
            "Please wait for the current operation to complete.",
            "Operation in Progress", 
            MB_OK | MB_ICONINFORMATION);
        return;
    }
    
    EndDialog(m_hDlg, IDOK);
}

void CUpdateDialog::AddUpdateFile(const UpdateFileInfo& fileInfo) {
    m_UpdateFiles.push_back(fileInfo);
}

void CUpdateDialog::ClearUpdateFiles() {
    m_UpdateFiles.clear();
    ListView_DeleteAllItems(m_hListView);
}

void CUpdateDialog::RefreshList() {
    ListView_DeleteAllItems(m_hListView);
    
    for (size_t i = 0; i < m_UpdateFiles.size(); i++) {
        const auto& file = m_UpdateFiles[i];
        
        LVITEM lvi = {0};
        lvi.mask = LVIF_TEXT;
        lvi.iItem = i;
        
        // File name
        lvi.pszText = (LPSTR)file.fileName.c_str();
        ListView_InsertItem(m_hListView, &lvi);
        
        // Current version
        ListView_SetItemText(m_hListView, i, 1, (LPSTR)file.currentVersion.c_str());
        
        // Latest version
        ListView_SetItemText(m_hListView, i, 2, (LPSTR)file.version.c_str());
        
        // File size
        char sizeStr[32];
        if (file.fileSize < 1024) {
            sprintf_s(sizeStr, "%d B", file.fileSize);
        } else if (file.fileSize < 1024 * 1024) {
            sprintf_s(sizeStr, "%.1f KB", file.fileSize / 1024.0f);
        } else {
            sprintf_s(sizeStr, "%.2f MB", file.fileSize / (1024.0f * 1024.0f));
        }
        ListView_SetItemText(m_hListView, i, 3, sizeStr);
        
        // Status
        ListView_SetItemText(m_hListView, i, 4, (LPSTR)GetStatusText(file.status));
    }
}

void CUpdateDialog::SetStatus(const char* status) {
    SetWindowTextA(m_hStatusText, status);
}

void CUpdateDialog::UpdateProgress(int current, int total, const char* status) {
    SendMessage(m_hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, total));
    SendMessage(m_hProgress, PBM_SETPOS, current, 0);
    SetStatus(status);
}

void CUpdateDialog::EnableButtons(bool check, bool download, bool apply, bool verify) {
    EnableWindow(GetDlgItem(m_hDlg, IDC_BTN_CHECK), check);
    EnableWindow(GetDlgItem(m_hDlg, IDC_BTN_DOWNLOAD), download);
    EnableWindow(GetDlgItem(m_hDlg, IDC_BTN_APPLY), apply);
    EnableWindow(GetDlgItem(m_hDlg, IDC_BTN_VERIFY), verify);
}

const char* CUpdateDialog::GetStatusText(FileUpdateStatus status) {
    switch (status) {
        case FILE_STATUS_PENDING: return "Pending";
        case FILE_STATUS_CHECKING: return "Checking...";
        case FILE_STATUS_AVAILABLE: return "Update Available";
        case FILE_STATUS_DOWNLOADING: return "Downloading...";
        case FILE_STATUS_DOWNLOADED: return "Downloaded";
        case FILE_STATUS_VERIFYING: return "Verifying...";
        case FILE_STATUS_READY: return "Ready";
        case FILE_STATUS_APPLIED: return "Applied";
        case FILE_STATUS_ERROR: return "Error";
        case FILE_STATUS_UPTODATE: return "Up to Date";
        default: return "Unknown";
    }
}

COLORREF CUpdateDialog::GetStatusColor(FileUpdateStatus status) {
    switch (status) {
        case FILE_STATUS_APPLIED: return RGB(0, 180, 0);
        case FILE_STATUS_READY: return RGB(0, 120, 215);
        case FILE_STATUS_ERROR: return RGB(232, 17, 35);
        case FILE_STATUS_AVAILABLE: return RGB(255, 140, 0);
        case FILE_STATUS_UPTODATE: return RGB(100, 100, 100);
        default: return RGB(0, 0, 0);
    }
}