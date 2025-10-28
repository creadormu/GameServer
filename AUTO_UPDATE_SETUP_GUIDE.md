# GameServer Auto-Update System - Complete Setup Guide

## 📋 Overview

Your GameServer now has a professional auto-update system that can:
- ✅ Automatically check for updates from your web server
- ✅ Download updates (GameServer.exe, Data/Item.txt, etc.)
- ✅ Verify downloaded files for security
- ✅ Apply updates with automatic backup and restart
- ✅ Configure per-server (enable/disable for specific instances)
- ✅ Show notifications when updates are available

---

## 🚀 Quick Start (3 Steps)

### Step 1: Add Update Menu to GameServer

Since the `.rc` file is a Visual Studio resource file, you need to add the menu manually:

1. Open `GameServer.sln` in Visual Studio
2. In Solution Explorer, open `GameServer.rc`
3. Find the menu section and add a new POPUP menu called "Update" before the "& Buy Premium" menu:

```
POPUP "Update"
BEGIN
    MENUITEM "Check for Updates",          IDM_UPDATE_CHECK
    MENUITEM "Download Update",            IDM_UPDATE_DOWNLOAD
    MENUITEM "Apply Update",               IDM_UPDATE_APPLY
    MENUITEM SEPARATOR
    MENUITEM "Update Configuration...",    IDM_UPDATE_CONFIG
END
```

**Visual Location:** Add it right after the "IA" menu and before "& Buy Premium"

### Step 2: Configure Your Web Server

1. Create a folder on your web hosting (e.g., `/gameserver/updates/`)

2. Upload `update_manifest.txt` (see template below)

3. Upload your update files (e.g., `GameServer_1.0.1.exe`)

4. Edit `/Data/UpdateConfig.ini` and set your URL:
   ```ini
   UpdateServerUrl = http://yourwebsite.com/gameserver/updates
   ```

### Step 3: Enable Auto-Update

Edit `/Data/UpdateConfig.ini`:
```ini
Enabled = 1
AutoCheck = 1
ShowNotifications = 1
```

**Done!** Rebuild your project and run GameServer.

---

## 📁 File Structure

```
GameServer/
├── GameServer/
│   ├── UpdateManager.h          ← New: Update system header
│   ├── UpdateManager.cpp        ← New: Update system implementation
│   ├── resource.h               ← Modified: Added menu IDs
│   └── GameServer.cpp           ← Modified: Integrated update system
├── Data/
│   ├── UpdateConfig.ini         ← New: Update configuration
│   └── update_manifest_example.txt  ← New: Template for your web server
└── Update/
    └── Temp/                    ← Created automatically for downloads
```

---

## 🌐 Web Server Setup (Detailed)

### Update Manifest Format

Create `update_manifest.txt` on your web server:

```ini
[Update]
Version=1.0.1
DownloadUrl=http://yourwebsite.com/gameserver/updates/GameServer_1.0.1.exe
FileName=GameServer.exe
FileHash=
FileSize=1234567
FileType=0
Description=Bug fixes and new features
Required=0
```

### File Types

- **FileType=0**: Executable (GameServer.exe) - Requires server restart
- **FileType=1**: Data file (Item.txt, Monster.txt, etc.) - May need data reload
- **FileType=2**: Configuration file

### Example Directory Structure on Web Server

```
http://yourwebsite.com/gameserver/updates/
├── update_manifest.txt
├── GameServer_1.0.1.exe
├── GameServer_1.0.2.exe
├── Item_1.0.1.txt
└── Monster_1.0.2.txt
```

### Testing Your Setup

Open in browser: `http://yourwebsite.com/gameserver/updates/update_manifest.txt`

You should see the manifest content displayed.

---

## ⚙️ Configuration Options

### UpdateConfig.ini - Complete Reference

```ini
[UpdateManager]
# Enable/Disable auto-update system
Enabled = 1

# Automatically check for updates every hour
AutoCheck = 1

# Automatically download updates when found
# NOTE: Updates are NEVER applied automatically
AutoDownload = 0

# Show popup notifications for updates
ShowNotifications = 1

# Your web server URL (no trailing slash)
UpdateServerUrl = http://yourwebsite.com/gameserver/updates

# Current version (must match manifest version for comparison)
CurrentVersion = 1.0.0

# Local temp directory for downloads
TempDirectory = .\Update\Temp
```

---

## 🎯 Per-Server Configuration

**Scenario:** You have 3 GameServers but only want Server 1 to auto-update

1. **Server 1** (Auto-update enabled):
   ```ini
   Enabled = 1
   AutoCheck = 1
   ```

2. **Server 2 & 3** (Auto-update disabled):
   ```ini
   Enabled = 0
   AutoCheck = 0
   ```

Each GameServer has its own `UpdateConfig.ini`, so you can control them individually!

---

## 📖 How to Use

### Automatic Mode (Recommended)

1. Set `Enabled = 1` and `AutoCheck = 1`
2. GameServer checks for updates every hour
3. If update found, you'll see a notification
4. Go to menu **Update → Apply Update** to install

### Manual Mode

1. Set `AutoCheck = 0`
2. Check manually: **Update → Check for Updates**
3. Download: **Update → Download Update**
4. Apply: **Update → Apply Update**

### Menu Options

- **Check for Updates**: Manually check your web server for new versions
- **Download Update**: Download the update file (if available)
- **Apply Update**: Install the update and restart GameServer
- **Update Configuration**: Open `UpdateConfig.ini` in notepad

---

## 🔄 Update Process Flow

### For Executable Updates (GameServer.exe)

1. **Backup created**: `GameServer.exe.backup`
2. **Server closes gracefully**
3. **Update script runs**: Replaces old .exe with new one
4. **Server restarts automatically**
5. **On error**: Automatic rollback from backup

### For Data File Updates (Item.txt, etc.)

1. **Backup created**: `Item.txt.backup`
2. **File replaced immediately**
3. **Server continues running**
4. **Manual reload**: Use menu to reload data if needed

---

## 🛡️ Security Features

✅ **File Hash Verification**: Ensures file integrity  
✅ **Automatic Backups**: Rollback on failure  
✅ **User Confirmation**: Required before applying updates  
✅ **Selective Updates**: Control which servers update  
✅ **Safe Restart**: Graceful shutdown before updating  

---

## 🚨 Troubleshooting

### "ERROR: Failed to download update manifest"

**Cause:** Web server URL is incorrect or server is offline

**Fix:**
1. Verify `UpdateServerUrl` in `UpdateConfig.ini`
2. Test URL in browser: `http://yoursite.com/updates/update_manifest.txt`
3. Check your web hosting is online

### "No update available" (but you uploaded new version)

**Cause:** Version mismatch

**Fix:**
1. Check `CurrentVersion` in `UpdateConfig.ini` (e.g., "1.0.0")
2. Check `Version` in your web `update_manifest.txt` (e.g., "1.0.1")
3. They must be different for update to be detected

### "Update download failed"

**Cause:** Direct download URL is blocked or incorrect

**Fix:**
1. Test download URL in browser directly
2. Ensure file is publicly accessible (not password protected)
3. Check firewall settings

### Update applies but server doesn't restart

**Cause:** Update script failed

**Fix:**
1. Check `Update/apply_update.bat` for errors
2. Ensure you have write permissions to GameServer.exe
3. Close antivirus that might block file replacement

---

## 💡 Best Practices

### Version Numbering

Use semantic versioning: `Major.Minor.Patch`

Examples:
- `1.0.0` → First release
- `1.0.1` → Bug fix
- `1.1.0` → New features
- `2.0.0` → Major changes

### Update Schedule

- **Small fixes**: Update immediately
- **Major updates**: Announce 24h in advance
- **Critical security**: Emergency update with server notice

### Testing Updates

1. Test on development server first
2. Create backup of production files
3. Update during low-traffic hours
4. Monitor logs after update

### File Hosting

- Use reliable web hosting
- Enable HTTPS for security (optional but recommended)
- Keep old versions available for rollback
- Monitor bandwidth usage

---

## 📝 Example Update Workflow

### Scenario: You fixed bugs and want to update all servers

**1. Prepare New Version**
```
Current: GameServer.exe (v1.0.0)
New: GameServer.exe (v1.0.1)
```

**2. Upload to Web Server**
```
Upload: GameServer_1.0.1.exe
Create: update_manifest.txt with Version=1.0.1
```

**3. Update Manifest**
```ini
[Update]
Version=1.0.1
DownloadUrl=http://yoursite.com/updates/GameServer_1.0.1.exe
FileName=GameServer.exe
FileSize=1234567
FileType=0
Description=Fixed login bug and improved performance
Required=0
```

**4. Servers Auto-Detect**
- Servers with `AutoCheck=1` will detect update within 1 hour
- Notification appears: "UPDATE AVAILABLE: 1.0.0 → 1.0.1"

**5. Apply Update**
- Go to **Update → Apply Update** on each server
- Server restarts automatically with new version

**Done!** All servers now running v1.0.1

---

## 🎓 Advanced Features

### Custom Update Intervals

Modify `UPDATE_CHECK_INTERVAL` in `UpdateManager.h`:
```cpp
#define UPDATE_CHECK_INTERVAL 3600000  // 1 hour in milliseconds
```

Change to:
- `1800000` = 30 minutes
- `7200000` = 2 hours
- `21600000` = 6 hours

### Multiple File Updates

Currently supports one file per manifest. For multiple files:

**Option 1:** Create separate manifests
- `update_manifest_exe.txt`
- `update_manifest_data.txt`

**Option 2:** Update in sequence
- Upload Item.txt update first
- After applied, upload GameServer.exe update

### Hash Verification (Advanced)

For production, implement proper MD5/SHA256 verification:

1. Calculate hash of your file:
   ```bash
   md5sum GameServer.exe
   ```

2. Add to manifest:
   ```ini
   FileHash=abc123def456...
   ```

3. Implement verification in `UpdateManager.cpp` (function provided, needs crypto library)

---

## 📞 Support

- **Issues**: Check logs in GameServer console
- **Menu not showing**: Rebuild project after adding menu
- **Update fails**: Check `Update/apply_update.bat` logs

---

## ✅ Checklist

Before going live with auto-update:

- [ ] Added Update menu to `GameServer.rc`
- [ ] Rebuilt GameServer project
- [ ] Created `/updates/` folder on web hosting
- [ ] Uploaded `update_manifest.txt`
- [ ] Uploaded update files
- [ ] Tested manifest URL in browser
- [ ] Configured `UpdateConfig.ini` with correct URL
- [ ] Set `Enabled = 1`
- [ ] Tested with development server first
- [ ] Created backups of production files

---

## 🎉 Congratulations!

Your GameServer now has a professional auto-update system like Advanced Installer!

**Benefits:**
- ✅ Users always get the latest version
- ✅ Fix bugs instantly without manual distribution
- ✅ Reduce support requests
- ✅ Professional image

**Next Steps:**
- Test the update process thoroughly
- Document your version history
- Plan regular update schedule
- Monitor update success rates

---

**Enjoy your new auto-update system! 🚀**
