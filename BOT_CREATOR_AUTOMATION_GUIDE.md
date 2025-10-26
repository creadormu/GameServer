# Bot Creator Automation System - Implementation Guide

## Overview
The bot creator system has been enhanced with **full automation** capabilities. Users no longer need to manually execute SQL queries or update XML files. Everything can be done with simple button clicks.

## What's New

### 1. **Automatic SQL Execution**
- Execute `CreateBots.sql` directly from the dialog
- No need to open SQL Management Studio
- Uses Windows Authentication (trusted connection)
- Provides clear success/error messages
- Creates execution logs in `IA\Generated\SQLOutput.log`

### 2. **Automatic Accounts.xml Update**
- Two modes: **UPDATE** or **REPLACE**
- **UPDATE Mode**: Merges new bots with existing ones
  - Keeps all existing bots
  - Adds new bot accounts
  - Updates duplicate accounts
  - Perfect for adding more bots incrementally
  
- **REPLACE Mode**: Complete replacement
  - Removes all old bots
  - Uses only the newly created bots
  - Good for fresh starts

- Automatic backups created at `IA\Accounts_Backup.xml`
- Shows detailed statistics (bots added, updated)

### 3. **Simplified Workflow**
**Old workflow** (5 steps, manual work):
1. Click Create Bots → generates files
2. Open SQL Server Management Studio
3. Execute CreateBots.sql manually
4. Manually copy/merge IA_Accounts.xml to Accounts.xml
5. Reload IA and Add Fake Online

**New workflow** (3 simple clicks):
1. Click **"Create Bots"** → generates files
2. Click **"Execute SQL"** → adds bots to database
3. Click **"Update Accounts.xml"** → merges configuration
4. Done! Just reload and add IA from menu

## Usage Instructions

### Step 1: Configure Your Bots
1. Open GameServer.exe
2. Go to menu: **FakeOnline → Create Bots**
3. Configure:
   - Bot count (1-1000)
   - Starting number
   - Level range
   - Location (gate or custom coords)
   - Bot behavior settings
   - Select class configurations to use

### Step 2: Create Bot Files
1. Click **"Create Bots"** button
2. Wait for success message
3. Files generated:
   - `IA\Generated\IA_Accounts.xml`
   - `IA\Generated\CreateBots.sql`

### Step 3: Execute SQL (NEW!)
1. Click **"Execute SQL"** button
2. Confirm the operation
3. SQL Server settings (optional, uses defaults):
   - Server: `.\\SQLEXPRESS` (default)
   - Database: `MuOnline` (default)
4. Wait for completion
5. Bots are now in your database!

### Step 4: Update Accounts.xml (NEW!)
1. Click **"Update Accounts.xml"** button
2. Choose mode:
   - **YES (Update)**: Keep existing bots, add new ones
   - **NO (Replace)**: Remove all old bots, use only new ones
   - **Cancel**: Abort operation
3. Wait for completion
4. Configuration is now ready!

### Step 5: Activate Bots
1. Go to GameServer menu
2. Click **"Reload IA Data"**
3. Click **"Add Fake Online"**
4. Your bots are now online!

## SQL Server Configuration

### Default Settings
- Server: `.\\SQLEXPRESS`
- Database: `MuOnline`
- Authentication: Windows (Trusted Connection)

### Custom Settings
You can add edit controls to the dialog (in the .rc file) for custom server/database:
- `IDC_EDIT_SQLSERVER` (ID: 3320)
- `IDC_EDIT_SQLDB` (ID: 3321)

The system will use these values if provided, otherwise uses defaults.

## File Locations

### Generated Files
- `IA\Generated\IA_Accounts.xml` - Bot configuration XML
- `IA\Generated\CreateBots.sql` - SQL script to create bots
- `IA\Generated\SQLOutput.log` - SQL execution log

### Target Files
- `IA\Accounts.xml` - Main bot configuration (updated automatically)
- `IA\Accounts_Backup.xml` - Automatic backup before updates

## Error Handling

### SQL Execution Errors
If SQL execution fails:
1. Check that SQL Server is running
2. Verify SQLCMD is installed (comes with SQL Server)
3. Check connection settings
4. Review `IA\Generated\SQLOutput.log` for details

### XML Update Errors
If XML update fails:
1. Check file permissions
2. Verify source file exists: `IA\Generated\IA_Accounts.xml`
3. Check XML format is valid
4. Backup is always created before attempting update

## Technical Details

### New Functions Added

#### ExecuteSQLFile()
```cpp
bool ExecuteSQLFile(const char* sqlFilePath, const char* serverName, 
                    const char* databaseName, char* errorMsg, int errorMsgSize)
```
- Uses SQLCMD command-line tool
- 60-second timeout
- Creates hidden process
- Captures exit code and logs

#### UpdateAccountsXML()
```cpp
bool UpdateAccountsXML(bool replaceMode, char* errorMsg, int errorMsgSize)
```
- Uses pugixml for XML parsing
- Handles duplicate accounts intelligently
- Creates automatic backups
- Provides detailed statistics

### New Dialog Controls (Resource IDs)
- `IDC_BTN_EXECUTESQL` (3318) - Execute SQL button
- `IDC_BTN_UPDATEXML` (3319) - Update XML button  
- `IDC_EDIT_SQLSERVER` (3320) - SQL Server name (optional)
- `IDC_EDIT_SQLDB` (3321) - Database name (optional)

### Modified Files
1. **GameServer.cpp**
   - Added `ExecuteSQLFile()` function
   - Added `UpdateAccountsXML()` function
   - Enhanced `CreateBotsDialogProc()` with new button handlers
   - Added pugixml includes

2. **GameServer.h**
   - Added function declarations

3. **resource.h**
   - Added new control IDs

## Benefits

✅ **No SQL Knowledge Required** - Users don't need to know how to run SQL queries

✅ **Safer** - Automatic backups before any changes

✅ **Faster** - Complete automation in 3 clicks instead of manual steps

✅ **Error-Proof** - Clear error messages and validation

✅ **Flexible** - Choose to update or replace existing bots

✅ **Transparent** - Shows exactly what was done (added X, updated Y)

## Troubleshooting

### "SQLCMD not found"
**Solution**: Install SQL Server tools. SQLCMD comes with:
- SQL Server (any edition)
- SQL Server Management Studio
- SQL Server Command Line Utilities

### "Access denied" errors
**Solution**: 
- Run GameServer as Administrator
- Check Windows Authentication is enabled in SQL Server
- Verify your Windows account has database permissions

### Bots not appearing after activation
**Solution**:
1. Check SQL execution was successful
2. Verify Accounts.xml was updated
3. Make sure you clicked "Reload IA Data"
4. Then click "Add Fake Online"

## Future Enhancements (Optional)

Possible improvements:
- [ ] Progress bar for SQL execution
- [ ] SQL Authentication option (username/password)
- [ ] Batch processing indicator
- [ ] Preview mode before updating XML
- [ ] Rollback functionality
- [ ] Multi-server support

## Support

For issues or questions:
1. Check console logs in GameServer
2. Review `IA\Generated\SQLOutput.log`
3. Verify all files are in correct locations
4. Check SQL Server is running and accessible

---

**Version**: 1.0  
**Date**: 2025-10-26  
**Compatibility**: All GameServer versions with FakeOnline system
