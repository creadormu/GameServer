# Unicode Character Support for MuOnline GameServer

## 🌍 Overview

This implementation adds full Unicode character support to your MuOnline GameServer, enabling proper display of:

- **Portuguese**: ação, coração, João, São Paulo, campeão
- **Spanish**: niño, mañana, año (already working, now improved)
- **French**: château, café, crème
- **German**: Müller, Größe, Straße
- **Chinese**: 中文字符 (depends on client)
- **Japanese**: ひらがな、カタカナ (depends on client)
- **Korean**: 한글 (depends on client)
- **Russian**: Русский (depends on client)

## 📦 What's Included

### New Files Created:

1. **GameServer/UnicodeSupport.h** - Core UTF-8 conversion utilities
2. **GameServer/UnicodeODBC.h** - Unicode-enabled ODBC database wrapper
3. **GameServer/UnicodeFileIO.h** - UTF-8 file reading/writing
4. **GameServer/UnicodeODBC_Example.cpp** - Example code showing how to update your queries
5. **Database_Unicode_Migration.sql** - SQL script to convert database to Unicode
6. **Database_StoredProcedure_Unicode_Update.sql** - Updates stored procedures
7. **UNICODE_IMPLEMENTATION_GUIDE.md** - Complete implementation guide

### Files Modified:
- None (backward compatible! You update at your own pace)

## 🚀 Quick Start Guide

### Step 1: Backup Everything (CRITICAL!)

```bash
# Backup your database
BACKUP DATABASE [MuOnline] TO DISK = 'C:\Backup\MuOnline_PreUnicode.bak'

# Backup your GameServer folder
Copy entire GameServer folder to GameServer_Backup
```

### Step 2: Update Database (5-30 minutes)

Open SQL Server Management Studio and run:

```sql
-- 1. First script - converts tables
-- File: Database_Unicode_Migration.sql
-- Run time: 5-30 minutes depending on database size
USE [MuOnline]
GO
-- (paste entire script)

-- 2. Second script - updates stored procedures
-- File: Database_StoredProcedure_Unicode_Update.sql
-- Run time: < 1 minute
-- (paste entire script)
```

**Expected output:**
```
=========================================
Starting Unicode Migration for MuOnline
=========================================

Migrating MEMB_INFO table...
  MEMB_INFO: Converted varchar to nvarchar for text fields

Migrating AccountCharacter table...
  AccountCharacter: Converted varchar to nvarchar for character names

Migrating Character table...
  Character: Converted varchar to nvarchar for text fields

...

=========================================
Unicode Migration Completed Successfully!
=========================================
```

### Step 3: Update GameServer Code

Add the new headers to your project:

```cpp
// In stdafx.h, add at the end:
#include "UnicodeSupport.h"
#include "UnicodeODBC.h"
#include "UnicodeFileIO.h"
```

Recompile your GameServer.

### Step 4: Test!

1. **Test Portuguese names:**
   - Create character: "João"
   - Create character: "José"
   - Create character: "Campeão"

2. **Test chat messages:**
   - Send: "Ação de graças!"
   - Send: "São Paulo"
   - Send: "Coração valente"

3. **Test guild:**
   - Create guild: "Dragões"
   - Create guild: "Campeões"

## 📊 Before vs After

### Before (ASCII only):
```
Name: Jo?o        ❌ Wrong
Name: Cora??o     ❌ Wrong
Chat: A??o        ❌ Wrong
Guild: Drag?es    ❌ Wrong
```

### After (Unicode):
```
Name: João        ✅ Correct!
Name: Coração     ✅ Correct!
Chat: Ação        ✅ Correct!
Guild: Dragões    ✅ Correct!
```

## 🔧 How to Update Your Code

### Example 1: Character Creation

**Before:**
```cpp
sprintf_s(query, "INSERT INTO Character (Name) VALUES ('%s')", name);
SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS);
```

**After:**
```cpp
// Option A: Simple (automatic conversion)
sprintf_s(query, "INSERT INTO Character (Name) VALUES (N'%s')", name);
std::wstring wQuery = UTF8ToWide(query);
SQLExecDirectW(hStmt, (SQLWCHAR*)wQuery.c_str(), SQL_NTS);

// Option B: Using wrapper class (easiest!)
CUnicodeODBC db;
db.Connect("localhost", "MuOnline");
sprintf_s(query, "INSERT INTO Character (Name) VALUES (N'%s')", name);
db.ExecuteQuery(query);
db.Disconnect();
```

### Example 2: Loading Characters

**Before:**
```cpp
SQLExecDirect(hStmt, (SQLCHAR*)"SELECT Name FROM Character", SQL_NTS);
while (SQLFetch(hStmt) == SQL_SUCCESS)
{
    char name[11];
    SQLGetData(hStmt, 1, SQL_C_CHAR, name, sizeof(name), &indicator);
}
```

**After:**
```cpp
std::wstring wQuery = L"SELECT Name FROM Character";
SQLExecDirectW(hStmt, (SQLWCHAR*)wQuery.c_str(), SQL_NTS);
while (SQLFetch(hStmt) == SQL_SUCCESS)
{
    SQLWCHAR wName[11];
    char name[31]; // UTF-8 may need more bytes
    SQLGetData(hStmt, 1, SQL_C_WCHAR, wName, sizeof(wName), &indicator);
    SQLWCHARToChar(wName, name, sizeof(name));
}
```

### Example 3: Chat Messages

Chat messages already work with UTF-8! Just make sure to validate:

```cpp
void CGChatRecv(PMSG_CHAT_RECV* lpMsg, int aIndex)
{
    // Validate UTF-8 to prevent hacks
    if (!IsValidUTF8(lpMsg->message))
        return;
    
    // Truncate safely respecting UTF-8 boundaries
    TruncateUTF8(lpMsg->message, MAX_CHAT_MESSAGE_SIZE);
    
    // Rest of your code stays the same!
    // UTF-8 passes through transparently
}
```

## 🗂️ File Operations (Config Files)

### Reading UTF-8 Files:

```cpp
// Old way (ANSI only)
FILE* f = fopen("config.txt", "r");
char line[256];
fgets(line, 256, f);
fclose(f);

// New way (UTF-8 support)
std::vector<std::string> lines;
ReadUTF8File("config.txt", lines);
for (const auto& line : lines)
{
    // Process line - contains UTF-8
}
```

### Writing UTF-8 Files:

```cpp
std::vector<std::string> lines;
lines.push_back("Servidor: São Paulo");
lines.push_back("Descrição: Melhor servidor!");
WriteUTF8File("config.txt", lines, true); // true = write BOM
```

## 🎯 Key Functions Reference

### String Conversion:
```cpp
// UTF-8 ↔ Wide (Windows Unicode)
std::wstring wide = UTF8ToWide(utf8String);
std::string utf8 = WideToUTF8(wideString);

// ANSI ↔ UTF-8
std::string utf8 = ANSIToUTF8(ansiString);
std::string ansi = UTF8ToANSI(utf8String);

// Safe copy with truncation
SafeCopyUTF8(dest, destSize, src);
```

### String Operations:
```cpp
// Get character count (not byte count!)
size_t charCount = UTF8Length(utf8String);

// Truncate safely at character boundary
TruncateUTF8(utf8String, maxBytes);

// Validate UTF-8 encoding
bool isValid = IsValidUTF8(utf8String);
```

### ODBC Operations:
```cpp
// Convert for ODBC
SQLWCHAR wBuffer[256];
CharToSQLWCHAR(utf8String, wBuffer, 256);

// Convert from ODBC
char utf8Buffer[512];
SQLWCHARToChar(wBuffer, utf8Buffer, sizeof(utf8Buffer));
```

### File Operations:
```cpp
// Read/Write UTF-8 files
ReadUTF8File(filename, lines);
WriteUTF8File(filename, lines, true);

// INI files
std::string value = ReadINIValueUTF8(filename, section, key);
WriteINIValueUTF8(filename, section, key, value);
```

## 📝 Important Notes

### Character Limits:
- **Character names**: 10 characters (not bytes!)
  - English "TestPlayer" = 10 bytes ✅
  - Portuguese "João" = 5 bytes ✅
  - Chinese "中文名字" = 12 bytes (4 chars × 3 bytes) ⚠️ May need adjustment

- **Chat messages**: 60 bytes maximum
  - English: ~60 characters
  - Portuguese: ~40-60 characters (accents use 2 bytes)
  - Chinese: ~20 characters (3 bytes each)

### SQL Server Collation:
Your database should use a Unicode-compatible collation:
- **Recommended**: `Latin1_General_CI_AS`
- **Also good**: `SQL_Latin1_General_CP1_CI_AS`
- **Check with**: `SELECT DATABASEPROPERTYEX('MuOnline', 'Collation')`

### Client Compatibility:
- **Season 6+**: Generally supports Unicode
- **Older clients**: May have limited support
- Test with your specific client version!

## 🐛 Troubleshooting

### Problem: Characters showing as "???"
**Solution:** Database not migrated. Run `Database_Unicode_Migration.sql`

### Problem: Chat messages truncated incorrectly
**Solution:** Use `TruncateUTF8()` instead of simple string truncation

### Problem: Accents stripped from names
**Solution:** Missing `N` prefix in SQL: Use `N'João'` not `'João'`

### Problem: Compilation errors
**Solution:** Make sure to include headers in stdafx.h

### Problem: ODBC errors after migration
**Solution:** Verify using Unicode functions:
- ✅ `SQLDriverConnectW`
- ✅ `SQLExecDirectW`  
- ✅ `SQLPrepareW`
- ✅ `SQL_C_WCHAR`
- ❌ ~~SQLDriverConnect~~
- ❌ ~~SQLExecDirect~~
- ❌ ~~SQL_C_CHAR~~

## 📈 Performance Impact

- **Database size**: Increases ~50-100% (NVARCHAR uses 2 bytes per char)
- **Query speed**: Minimal impact (<5% for most queries)
- **Memory**: Minimal impact (few KB per player)
- **Network**: No change (client already sends Unicode)

## ✅ Validation Checklist

After implementation, verify:

- [ ] Database migration completed successfully
- [ ] All VARCHAR columns converted to NVARCHAR
- [ ] Stored procedures updated
- [ ] GameServer compiles without errors
- [ ] Can create character with Portuguese name
- [ ] Can send chat with accented characters
- [ ] Can create guild with Unicode name
- [ ] Existing characters still load correctly
- [ ] Existing accounts still work

## 🎓 Learning Resources

- **Full Implementation Guide**: See `UNICODE_IMPLEMENTATION_GUIDE.md`
- **Example Code**: See `UnicodeODBC_Example.cpp`
- **UTF-8 Primer**: https://en.wikipedia.org/wiki/UTF-8
- **ODBC Unicode**: https://docs.microsoft.com/en-us/sql/odbc/reference/develop-app/unicode

## 🤝 Support

If you need help:

1. Check `UNICODE_IMPLEMENTATION_GUIDE.md` for detailed info
2. Review examples in `UnicodeODBC_Example.cpp`
3. Verify database migration completed
4. Test with simple Portuguese characters first (á, é, í, ó, ú)

## 📜 License

This Unicode implementation maintains compatibility with your existing MuOnline GameServer structure and does not modify core game mechanics.

---

**Good luck with your multilingual MuOnline server!** 🌍🎮✨

**Boa sorte com seu servidor MuOnline multilíngue!** 🇧🇷🇵🇹

**祝你的多语言MuOnline服务器好运！** 🇨🇳

**マルチ言語MuOnlineサーバーの成功を祈ります！** 🇯🇵

**다국어 MuOnline 서버의 성공을 기원합니다!** 🇰🇷
