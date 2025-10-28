================================================================================
WEB SERVER SETUP - Upload These Files to Your Hosting
================================================================================

STEP 1: Create Directory Structure
-----------------------------------
On your web hosting, create a folder:
    /public_html/gameserver/updates/
    
(Or whatever path you prefer - just make sure it's publicly accessible via HTTP)


STEP 2: Upload Files
---------------------
Upload these files to that directory:

1. update_manifest.txt           (This file - contains version info)
2. GameServer_1.0.1.exe          (Your new GameServer executable)
3. Any other update files        (Item.txt, Monster.txt, etc.)


STEP 3: Edit update_manifest.txt
---------------------------------
Open update_manifest.txt and modify:

[Update]
Version=1.0.1                    ← Change to your new version
DownloadUrl=http://yourwebsite.com/gameserver/updates/GameServer_1.0.1.exe
                                 ↑ Change to YOUR domain
FileName=GameServer.exe          ← Keep as is (target filename)
FileHash=                        ← Optional (leave empty for now)
FileSize=1234567                 ← Right-click your .exe → Properties → Size in bytes
FileType=0                       ← Keep as 0 for .exe files
Description=Bug fixes and improvements  ← Describe what's new
Required=0                       ← 0=Optional, 1=Required


STEP 4: Test Access
--------------------
Open in your web browser:
    http://yourwebsite.com/gameserver/updates/update_manifest.txt

You should see the content displayed. If you get "404 Not Found":
- Check the file path
- Check file permissions (should be readable by everyone)
- Check if your hosting requires .htaccess rules


STEP 5: Configure GameServer
-----------------------------
On each GameServer, edit:
    Data/UpdateConfig.ini

Set:
    UpdateServerUrl = http://yourwebsite.com/gameserver/updates
                      ↑ NO trailing slash!


STEP 6: Enable Auto-Update
---------------------------
In Data/UpdateConfig.ini:
    Enabled = 1
    AutoCheck = 1


DONE! Your servers will now check for updates automatically.


================================================================================
EXAMPLE DIRECTORY STRUCTURE
================================================================================

Your web hosting folder should look like this:

/public_html/
    └── gameserver/
        └── updates/
            ├── update_manifest.txt
            ├── GameServer_1.0.1.exe
            ├── GameServer_1.0.2.exe   (future updates)
            ├── Item_1.0.1.txt         (data file updates)
            └── Monster_1.0.2.txt      (data file updates)

URL: http://yourwebsite.com/gameserver/updates/update_manifest.txt


================================================================================
UPDATING TO A NEW VERSION
================================================================================

When you want to push a new update:

1. Upload new file: GameServer_1.0.2.exe

2. Edit update_manifest.txt:
   [Update]
   Version=1.0.2                  ← Increment version
   DownloadUrl=http://yourwebsite.com/gameserver/updates/GameServer_1.0.2.exe
                                  ↑ Update filename
   FileName=GameServer.exe
   FileSize=1234567               ← Update file size
   Description=New features added  ← Update description

3. Save update_manifest.txt

4. Within 1 hour, all GameServers with AutoCheck=1 will detect the update!


================================================================================
MULTIPLE FILE UPDATES
================================================================================

To update data files (Item.txt, Monster.txt, etc.):

Create a separate manifest (or update sequentially):

[Update]
Version=1.0.2
DownloadUrl=http://yourwebsite.com/gameserver/updates/Item_1.0.2.txt
FileName=Item.txt
FileSize=524288
FileType=1                        ← Type 1 for data files
Description=New items added, balance changes
Required=0


================================================================================
SECURITY NOTES
================================================================================

- Keep your update server URL private (only share with trusted people)
- Use HTTPS if your hosting supports it (more secure)
- Regularly check your hosting logs for unusual download activity
- Keep backup of old versions in case rollback is needed
- Test updates on development server before pushing to production


================================================================================
TROUBLESHOOTING
================================================================================

Problem: GameServers can't download updates
Solution: 
- Check file permissions (should be 644 or readable by all)
- Verify URL is correct and accessible in browser
- Check hosting firewall rules

Problem: Download starts but fails partway
Solution:
- Check available disk space on hosting
- Verify file isn't corrupted
- Try re-uploading the file

Problem: Updates detected but wrong version shown
Solution:
- Clear browser cache
- Wait a few minutes for hosting cache to clear
- Verify update_manifest.txt has correct version number


================================================================================

That's all! Upload these files to your hosting and your auto-update system is ready!

================================================================================
