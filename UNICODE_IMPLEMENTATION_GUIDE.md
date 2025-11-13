# MuOnline GameServer - Unicode Character Support Implementation Guide

## Overview
This guide explains how to implement Unicode character support in your MuOnline GameServer to properly display Portuguese, Chinese, Japanese, Korean, and other languages with special characters.

## Current Status
- **Spanish & English**: Working perfectly (ASCII characters)
- **Portuguese**: Some words show incorrectly (accented characters like ã, õ, ç, á, é, í, ó, ú)
- **Chinese, Japanese, Korean**: Not supported yet (multibyte characters)

## Solution Architecture

### 1. Character Encoding Strategy
We use **UTF-8** encoding throughout the system because:
- Compatible with ASCII (English/Spanish continue to work)
- Supports all world languages
- Efficient storage (1-4 bytes per character)
- Industry standard for web and databases

### 2. Three-Layer Approach

```
┌─────────────────────────────────────────────┐
│  Client (Game Client)                        │
│  - Already sends UTF-8 or system encoding   │
└──────────────────┬──────────────────────────┘
                   │
┌──────────────────▼──────────────────────────┐
│  GameServer (C++)                            │
│  - Convert to UTF-8 internally              │
│  - Use new UnicodeSupport.h helpers         │
└──────────────────┬──────────────────────────┘
                   │
┌──────────────────▼──────────────────────────┐
│  Database (SQL Server)                       │
│  - Use NVARCHAR instead of VARCHAR          │
│  - Unicode-enabled stored procedures        │
└─────────────────────────────────────────────┘
```

## Implementation Steps

### Step 1: Database Migration (REQUIRED)

**⚠️ BACKUP YOUR DATABASE FIRST!**

Run these SQL scripts in order:

```sql
-- 1. Convert tables to Unicode
-- File: Database_Unicode_Migration.sql
USE [MuOnline]
GO
-- This converts VARCHAR → NVARCHAR for all text columns
-- Run time: 5-30 minutes depending on database size
```

```sql
-- 2. Update stored procedures
-- File: Database_StoredProcedure_Unicode_Update.sql
-- This updates procedures to use NVARCHAR parameters
```

**Important Tables Updated:**
- `MEMB_INFO` - Account names and personal info
- `AccountCharacter` - Character lists
- `Character` - Character names, language, married partners
- `Guild` - Guild names and notices
- `GuildMember` - Member names
- `ChatLog` - Chat messages (if exists)
- `MemoData` - Mail/messages (if exists)
- `PersonalShop` - Shop names (if exists)

### Step 2: GameServer Code Integration

#### 2.1 Add New Header Files

Copy these files to your `GameServer/` directory:
- `UnicodeSupport.h` - UTF-8 conversion utilities
- `UnicodeODBC.h` - Unicode-enabled database wrapper

#### 2.2 Update stdafx.h

Add at the end of `stdafx.h`:

```cpp
// Unicode support for multilingual characters
#include "UnicodeSupport.h"
#include "UnicodeODBC.h"
```

#### 2.3 Update Database Connection Code

**OLD CODE (ASCII only):**
```cpp
char connStr[512];
sprintf_s(connStr, sizeof(connStr),
    "DRIVER={SQL Server};SERVER=%s;DATABASE=%s;Trusted_Connection=yes;",
    server, database);

ret = SQLDriverConnect(g_hOdbcConn, NULL, (SQLCHAR*)connStr, SQL_NTS, 
                      NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
```

**NEW CODE (Unicode):**
```cpp
// Convert connection parameters to wide string
wchar_t wConnStr[512];
swprintf_s(wConnStr, 512, 
    L"DRIVER={SQL Server};SERVER=%s;DATABASE=%s;Trusted_Connection=yes;",
    UTF8ToWide(server).c_str(), UTF8ToWide(database).c_str());

// Use Unicode ODBC function
ret = SQLDriverConnectW(g_hOdbcConn, NULL, (SQLWCHAR*)wConnStr, SQL_NTS, 
                       NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
```

#### 2.4 Update SQL Queries

**OLD CODE:**
```cpp
const char* query = "SELECT Name FROM Character WHERE AccountID = ?";
ret = SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS);
```

**NEW CODE:**
```cpp
std::wstring wQuery = UTF8ToWide("SELECT Name FROM Character WHERE AccountID = ?");
ret = SQLExecDirectW(hStmt, (SQLWCHAR*)wQuery.c_str(), SQL_NTS);
```

#### 2.5 Reading Unicode Data from Database

**OLD CODE:**
```cpp
char name[11];
SQLGetData(hStmt, 1, SQL_C_CHAR, name, sizeof(name), &indicator);
```

**NEW CODE:**
```cpp
SQLWCHAR wName[11];
SQLGetData(hStmt, 1, SQL_C_WCHAR, wName, sizeof(wName), &indicator);

char name[31]; // UTF-8 may need more bytes than wide chars
SQLWCHARToChar(wName, name, sizeof(name));
```

### Step 3: Chat System Updates

The chat system needs minimal changes since it already uses char arrays. The key is ensuring UTF-8 is preserved:

**In Protocol.h (already correct structure):**
```cpp
struct PMSG_CHAT_RECV
{
    PBMSG_HEAD header; // C1:00
    char name[10];     // Keep as char - will contain UTF-8
    char message[60];  // Keep as char - will contain UTF-8
};
```

**In chat processing code:**
```cpp
void CGChatRecv(PMSG_CHAT_RECV* lpMsg, int aIndex)
{
    LPOBJ lpObj = &gObj[aIndex];
    
    // Validate UTF-8
    if (!IsValidUTF8(lpMsg->message))
    {
        // Invalid UTF-8, possibly hack attempt
        return;
    }
    
    // Truncate to ensure buffer safety while respecting UTF-8 boundaries
    TruncateUTF8(lpMsg->message, MAX_CHAT_MESSAGE_SIZE);
    
    // Process chat normally - UTF-8 passes through
    // ...
}
```

### Step 4: File Reading/Writing (Configuration Files)

For text files that may contain Unicode (notices, messages, config):

**Reading UTF-8 files:**
```cpp
bool LoadTextFile(const char* filename, std::vector<std::string>& lines)
{
    FILE* file = nullptr;
    if (fopen_s(&file, filename, "r, ccs=UTF-8") != 0)
        return false;
    
    wchar_t wLine[1024];
    while (fgetws(wLine, 1024, file) != nullptr)
    {
        std::string utf8Line = WideToUTF8(wLine);
        lines.push_back(utf8Line);
    }
    
    fclose(file);
    return true;
}
```

**Writing UTF-8 files:**
```cpp
bool SaveTextFile(const char* filename, const std::vector<std::string>& lines)
{
    FILE* file = nullptr;
    if (fopen_s(&file, filename, "w, ccs=UTF-8") != 0)
        return false;
    
    for (const auto& line : lines)
    {
        std::wstring wLine = UTF8ToWide(line.c_str());
        fwprintf(file, L"%s\n", wLine.c_str());
    }
    
    fclose(file);
    return true;
}
```

### Step 5: Character Name Validation

Update character name validation to support Unicode:

```cpp
bool IsValidCharacterName(const char* name)
{
    // Check if valid UTF-8
    if (!IsValidUTF8(name))
        return false;
    
    // Get character count (not byte count)
    size_t charCount = UTF8Length(name);
    
    // Name must be 3-10 characters
    if (charCount < 3 || charCount > 10)
        return false;
    
    // Allow letters, numbers, and some special chars
    // This check needs to be updated for Unicode ranges
    std::wstring wName = UTF8ToWide(name);
    
    for (size_t i = 0; i < wName.length(); i++)
    {
        wchar_t c = wName[i];
        
        // Allow:
        // - ASCII letters and numbers
        // - Latin extended (Portuguese: À-ÿ, range 0xC0-0xFF)
        // - CJK Unified Ideographs (Chinese: 0x4E00-0x9FFF)
        // - Hangul (Korean: 0xAC00-0xD7AF)
        // - Hiragana/Katakana (Japanese: 0x3040-0x30FF)
        
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || 
            (c >= '0' && c <= '9') ||
            (c >= 0xC0 && c <= 0xFF) ||      // Latin Extended
            (c >= 0x4E00 && c <= 0x9FFF) ||  // CJK
            (c >= 0xAC00 && c <= 0xD7AF) ||  // Korean
            (c >= 0x3040 && c <= 0x30FF))    // Japanese
        {
            continue;
        }
        else
        {
            return false; // Invalid character
        }
    }
    
    return true;
}
```

## Testing Checklist

### Database Testing
- [ ] Run migration scripts successfully
- [ ] Verify all VARCHAR columns converted to NVARCHAR
- [ ] Test stored procedures with Unicode parameters
- [ ] Create test character with Portuguese name (e.g., "João", "José")
- [ ] Create test character with accents (e.g., "Ação", "Campeão")

### GameServer Testing
- [ ] Compile GameServer with new headers
- [ ] Test login with existing accounts
- [ ] Create character with Portuguese name
- [ ] Create character with Chinese/Japanese/Korean name (if applicable)
- [ ] Send chat message with special characters
- [ ] Test guild creation with Unicode name
- [ ] Test personal shop with Unicode name
- [ ] Test character rename with Unicode
- [ ] Test mail/whisper with Unicode

### Client Testing
- [ ] Portuguese: Test "ação, coração, informação, João, São Paulo"
- [ ] Spanish: Verify still works "ñ, á, é, í, ó, ú"
- [ ] Chinese: Test if client supports (depends on client version)
- [ ] Japanese: Test if client supports (depends on client version)
- [ ] Korean: Test if client supports (depends on client version)

## Common Issues and Solutions

### Issue 1: Characters showing as "???" or boxes
**Cause:** Database still using VARCHAR instead of NVARCHAR  
**Solution:** Run Database_Unicode_Migration.sql script

### Issue 2: Accents stripped from names
**Cause:** ODBC using ASCII functions instead of Unicode  
**Solution:** Use SQLDriverConnectW, SQLExecDirectW, SQL_C_WCHAR

### Issue 3: Chat messages truncated incorrectly
**Cause:** Byte-level truncation cutting UTF-8 sequences  
**Solution:** Use TruncateUTF8() function to respect character boundaries

### Issue 4: Some characters work, others don't
**Cause:** Mixing encodings (UTF-8, ANSI, Wide)  
**Solution:** Be consistent - use UTF-8 throughout as char*, convert to wide only for Windows/ODBC APIs

### Issue 5: Client doesn't display Unicode
**Cause:** Old client version may not support Unicode  
**Solution:** Update client or check client's encoding settings

## Performance Considerations

- **Memory:** UTF-8 uses 1-4 bytes per character vs. 1 byte for ASCII
  - English/Spanish: Same (1 byte)
  - Portuguese: Mostly same (accents use 2 bytes)
  - Chinese/Japanese/Korean: 3 bytes per character
  
- **Database:** NVARCHAR uses 2 bytes per character (fixed)
  - Index size will roughly double
  - Query performance impact: minimal (<5%)

- **Network:** No change - client already sends in its encoding

## Best Practices

1. **Always validate UTF-8** before processing strings
2. **Use UTF8Length()** instead of strlen() for character counts
3. **Use TruncateUTF8()** instead of simple truncation
4. **Store as UTF-8** (char*) internally, convert to wide (wchar_t*) only for Windows APIs
5. **Test with real Unicode** data, not just ASCII

## Example Code Snippets

### Creating Character with Unicode Name
```cpp
bool CreateCharacter(const char* accountID, const char* characterName, int classType)
{
    // Validate UTF-8 name
    if (!IsValidUTF8(characterName))
        return false;
    
    if (UTF8Length(characterName) < 3 || UTF8Length(characterName) > 10)
        return false;
    
    // Use Unicode ODBC
    CUnicodeODBC db;
    if (!db.Connect("localhost", "MuOnline"))
        return false;
    
    // Prepare Unicode query
    char query[512];
    sprintf_s(query, 
        "EXEC WZ_CreateCharacter @AccountID = N'%s', @Name = N'%s', @Class = %d",
        accountID, characterName, classType);
    
    bool result = db.ExecuteQuery(query);
    db.Disconnect();
    
    return result;
}
```

### Sending Chat Message with Unicode
```cpp
void SendChatMessage(LPOBJ lpObj, const char* message)
{
    // Validate and truncate
    char safeMsg[MAX_CHAT_MESSAGE_SIZE + 1];
    SafeCopyUTF8(safeMsg, sizeof(safeMsg), message);
    TruncateUTF8(safeMsg, MAX_CHAT_MESSAGE_SIZE);
    
    if (!IsValidUTF8(safeMsg))
        return;
    
    // Send to all players in viewport
    PMSG_CHAT_SEND pMsg;
    pMsg.header.set(0x00, sizeof(pMsg));
    pMsg.index[0] = SET_NUMBERHB(lpObj->Index);
    pMsg.index[1] = SET_NUMBERLB(lpObj->Index);
    SafeCopyUTF8(pMsg.message, sizeof(pMsg.message), safeMsg);
    
    for (int i = 0; i < MAX_VIEWPORT; i++)
    {
        int target = lpObj->VpPlayer[i].index;
        if (OBJECT_RANGE(target))
        {
            DataSend(target, (BYTE*)&pMsg, sizeof(pMsg));
        }
    }
}
```

## Support Matrix

| Language | Characters | Status | Notes |
|----------|-----------|--------|-------|
| English | A-Z, a-z, 0-9 | ✅ Working | Already supported |
| Spanish | ñ, á, é, í, ó, ú | ✅ Working | Already supported |
| Portuguese | ã, õ, ç, á, é, í, ó, ú | ⚠️ Fixed with migration | Main target |
| French | à, â, é, è, ê, ë, etc. | ✅ Will work | Same as Portuguese |
| German | ä, ö, ü, ß | ✅ Will work | Same as Portuguese |
| Russian | Cyrillic | ✅ Will work | If client supports |
| Chinese | 中文 | ⚠️ Depends on client | Requires client support |
| Japanese | ひらがな, カタカナ | ⚠️ Depends on client | Requires client support |
| Korean | 한글 | ⚠️ Depends on client | Requires client support |
| Arabic | العربية | ⚠️ Needs RTL support | Complex, not recommended |

## Migration Rollback Plan

If you need to rollback:

1. **Restore database backup** (fastest option)

2. **Or convert back to VARCHAR** (not recommended):
```sql
ALTER TABLE [dbo].[Character] ALTER COLUMN [Name] VARCHAR(10) NOT NULL
-- Repeat for all tables
-- ⚠️ Warning: Will lose Unicode characters!
```

3. **Revert GameServer** to backup version

## Additional Resources

- SQL Server Unicode: https://docs.microsoft.com/en-us/sql/relational-databases/collations/collation-and-unicode-support
- UTF-8 Encoding: https://en.wikipedia.org/wiki/UTF-8
- ODBC Unicode: https://docs.microsoft.com/en-us/sql/odbc/reference/develop-app/unicode

## Support

If you encounter issues:

1. Check database collation (should support Unicode)
2. Verify all VARCHAR converted to NVARCHAR
3. Ensure using SQLDriverConnectW not SQLDriverConnect
4. Test with simple Portuguese characters first (á, é, í, ó, ú)
5. Check client version supports Unicode

Good luck with your multilingual MuOnline server! 🌍🎮
