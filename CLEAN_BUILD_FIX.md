# 🔧 Clean Build Fix - Remove Modern Features

## ❌ Problem

Your GitHub version has **modern C++ features and duplicate files** causing compilation errors.

## ✅ Solution: Use ONLY These Files

Keep **ONLY** these auto-update files:

```
GameServer/
├── UpdateManager.h          ← Keep this ONLY
├── UpdateManager.cpp        ← Keep this ONLY
└── GameServer.cpp           ← Already modified
```

---

## 🗑️ Files to DELETE from Your Project

If you have **ANY** of these files, **DELETE THEM**:

❌ `UpdateDialog.h` - DELETE  
❌ `UpdateDialog.cpp` - DELETE  
❌ `UpdateSystem.h` - DELETE  
❌ `UpdateSystem.cpp` - DELETE  
❌ Any other Update* files except UpdateManager.h/cpp

---

## 🔍 Check for Duplicate Definitions

### Error: `'FileUpdateStatus' : 'enum' type redefinition`

**Cause:** You have FileUpdateStatus defined in multiple files

**Fix:** Only `UpdateManager.h` should have this enum:
```cpp
enum UpdateFileType {
    UPDATE_FILE_EXECUTABLE = 0,
    UPDATE_FILE_DATA = 1,
    UPDATE_FILE_CONFIG = 2
};
```

### Error: `'UpdateFileInfo' : 'struct' type redefinition`

**Cause:** You have UpdateFileInfo struct in multiple files

**Fix:** Only `UpdateManager.h` should have this struct.

---

## 🔧 Fix GameServer.cpp Errors

### Error: `'HandleUpdateMenu' : local function definitions are illegal`

**Problem:** You defined a function INSIDE WndProc

**Wrong:**
```cpp
LRESULT CALLBACK WndProc(...) {
    void HandleUpdateMenu() {  // ❌ WRONG!
        // ...
    }
}
```

**Correct:**
```cpp
// Define OUTSIDE WndProc, BEFORE it
void HandleUpdateMenu(HWND hWnd) {
    // ...
}

LRESULT CALLBACK WndProc(...) {
    // Just call it
    case IDM_UPDATE_CHECK:
        gUpdateManager.ManualCheckForUpdates();
        break;
}
```

**OR BETTER:** Just use direct calls like I did (no helper function needed):
```cpp
case IDM_UPDATE_CHECK:
    gUpdateManager.ManualCheckForUpdates();
    break;
case IDM_UPDATE_DOWNLOAD:
    if(gUpdateManager.IsUpdateAvailable()) {
        gUpdateManager.DownloadUpdate();
    } else {
        MessageBox(hWnd, "No update available...", "No Update", MB_OK);
    }
    break;
case IDM_UPDATE_APPLY:
    gUpdateManager.ApplyUpdate();
    break;
case IDM_UPDATE_CONFIG:
    ShellExecuteA(NULL, "open", ".\\Data\\UpdateConfig.ini", NULL, NULL, SW_SHOW);
    break;
```

---

## 📋 Step-by-Step Clean Build

### Step 1: Remove Extra Files

1. Go to your GameServer folder
2. Delete ANY file matching: `Update*.h`, `Update*.cpp` **EXCEPT** UpdateManager.h and UpdateManager.cpp
3. Make sure you have NO UpdateDialog files

### Step 2: Verify Your Files

You should have EXACTLY these update-related files:
- ✅ `GameServer/UpdateManager.h` (from my version)
- ✅ `GameServer/UpdateManager.cpp` (from my version)
- ✅ `GameServer/GameServer.cpp` (with my modifications)
- ✅ `GameServer/resource.h` (with menu IDs added)

### Step 3: Check GameServer.cpp

Make sure GameServer.cpp has:

```cpp
// At the top
#include "UpdateManager.h"

// In WinMain after class init
gUpdateManager.Init(hWnd);

// In WM_COMMAND switch
case IDM_UPDATE_CHECK:
    gUpdateManager.ManualCheckForUpdates();
    break;
case IDM_UPDATE_DOWNLOAD:
    if(gUpdateManager.IsUpdateAvailable()) {
        gUpdateManager.DownloadUpdate();
    } else {
        MessageBox(hWnd, "No update available to download.\n\nPlease check for updates first.", "No Update", MB_OK | MB_ICONINFORMATION);
    }
    break;
case IDM_UPDATE_APPLY:
    gUpdateManager.ApplyUpdate();
    break;
case IDM_UPDATE_CONFIG:
    ShellExecuteA(NULL, "open", ".\\Data\\UpdateConfig.ini", NULL, NULL, SW_SHOW);
    break;

// In WM_TIMER_10000
gUpdateManager.OnTimer();
```

### Step 4: NO Modern C++ Features

Make sure you have **ZERO** of these in UpdateManager files:
- ❌ `auto` keyword
- ❌ `std::string` usage
- ❌ Range-based for loops
- ❌ Lambdas
- ❌ Smart pointers

### Step 5: Rebuild

1. Clean solution
2. Rebuild all
3. Should compile with **ZERO** errors

---

## 🎯 What Your UpdateManager Should Look Like

### UpdateManager.h (Key Parts):

```cpp
#pragma once

#include <windows.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")

enum UpdateStatus {
    UPDATE_STATUS_NO_UPDATE = 0,
    UPDATE_STATUS_AVAILABLE = 1,
    UPDATE_STATUS_DOWNLOADING = 2,
    UPDATE_STATUS_READY = 3,
    UPDATE_STATUS_ERROR = 4,
    UPDATE_STATUS_DISABLED = 5
};

enum UpdateFileType {
    UPDATE_FILE_EXECUTABLE = 0,
    UPDATE_FILE_DATA = 1,
    UPDATE_FILE_CONFIG = 2
};

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

    void Init(HWND hWnd);
    void LoadConfig(const char* configFile);
    void SaveConfig(const char* configFile);

    bool CheckForUpdates();
    bool IsUpdateAvailable();
    UpdateInfo GetUpdateInfo();
    
    bool DownloadUpdate();
    bool VerifyDownloadedFile(const char* filePath, const char* expectedHash);
    bool ApplyUpdate();
    
    void ManualCheckForUpdates();
    
    void EnableAutoUpdate(bool enable);
    void OnTimer();
    
    bool IsEnabled() const { return m_Enabled; }
    void SetEnabled(bool enabled) { m_Enabled = enabled; }
    
    UpdateStatus GetStatus() const { return m_Status; }
    const char* GetStatusString() const;
    
    const char* GetCurrentVersion() const { return m_CurrentVersion; }
    void SetCurrentVersion(const char* version);
    
    const char* GetUpdateServerUrl() const { return m_UpdateServerUrl; }
    void SetUpdateServerUrl(const char* url);

    void ShowNotification(const char* message, UINT icon);
    void LogUpdate(const char* format, ...);

private:
    bool DownloadFile(const char* url, const char* destPath, DWORD* bytesDownloaded);
    bool ParseUpdateManifest(const char* manifestData);
    void CalculateFileHash(const char* filePath, char* outHash, int outHashSize);
    bool CreateBackup(const char* filePath);
    bool RestoreBackup(const char* filePath);
    void GetTempFilePath(const char* fileName, char* outPath, int outPathSize);
    
    static DWORD WINAPI DownloadThreadProc(LPVOID lpParam);
};

extern CUpdateManager gUpdateManager;
```

---

## ✅ Final Checklist

Before compiling:

- [ ] Deleted all Update* files except UpdateManager.h/cpp
- [ ] No `auto` keyword anywhere
- [ ] No `std::string` in UpdateManager files
- [ ] No functions defined inside WndProc
- [ ] No duplicate enum/struct definitions
- [ ] ShellExecute changed to ShellExecuteA
- [ ] #include <shellapi.h> at top of GameServer.cpp
- [ ] Menu IDs added to resource.h
- [ ] Clean solution + Rebuild

---

## 🔧 If Still Getting Errors

### Error about sprintf_s parameters:

**Wrong:**
```cpp
sprintf_s(buffer, "format", value);  // ❌
```

**Correct:**
```cpp
sprintf_s(buffer, sizeof(buffer), "format", value);  // ✅
```

### Error about missing braces:

**Check:** Every `{` has a matching `}`  
**Check:** Every function ends with `}`  
**Check:** Every switch/case ends properly  

---

## 📞 Quick Fix Commands

### To find duplicate definitions:
```
grep -r "enum FileUpdateStatus" GameServer/
grep -r "struct UpdateFileInfo" GameServer/
```

If you see **multiple results**, you have duplicates!

### To find Update files:
```
find GameServer/ -name "*Update*"
```

Delete everything **except** UpdateManager.h and UpdateManager.cpp

---

## 🎯 The SIMPLE Rule

**Your auto-update system needs:**
1. UpdateManager.h
2. UpdateManager.cpp
3. Modifications to GameServer.cpp
4. Menu IDs in resource.h
5. UpdateConfig.ini in Data folder

**THAT'S IT!** Nothing else!

---

## ✅ Final Test

After cleaning and rebuilding:

```cpp
// Should compile with ZERO errors
// Should see these in menu:
Update → Check for Updates
Update → Download Update  
Update → Apply Update
Update → Update Configuration
```

---

**If you follow this guide, your code will compile perfectly with your existing C++ standard!** 🚀
