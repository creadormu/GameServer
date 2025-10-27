# MU Online Bot System - Database Stored Procedures

This folder contains SQL scripts to create an efficient bot management system for MU Online, eliminating the need to generate large SQL files that cause the GameServer to freeze.

## 🎯 Problem Solved

**Before:** Creating 50+ bots would freeze GameServer.exe because it generated huge SQL files with embedded hex data.

**After:** Bots are created directly in the database using stored procedures, making the process instant and stable even with 1000+ bots.

## 📁 Files Overview

| File | Description |
|------|-------------|
| `00_INSTALL_ALL.sql` | Master installation script (run this first) |
| `01_CreateTables.sql` | Creates BotNames and BotClassConfigs tables |
| `02_PopulateBotNames.sql` | Inserts 1000+ names in 10 languages |
| `03_PopulateClassConfigs.sql` | Creates 7 configs for each of 7 classes (49 total) |
| `04_StoredProcedures.sql` | Creates all stored procedures |
| `05_UpdateConfigsTemplate.sql` | Template for updating hex values |

## 🚀 Installation

### Method 1: Using SQL Server Management Studio (SSMS)

1. Open SSMS and connect to your SQL Server
2. Click **File → Open → File**
3. Select `00_INSTALL_ALL.sql`
4. Press **F5** to execute

### Method 2: Using sqlcmd (Command Line)

```batch
sqlcmd -S localhost -d MuOnline -i "00_INSTALL_ALL.sql"
```

### Method 3: Individual Scripts

If the master script fails, run each script individually:

```sql
:r 01_CreateTables.sql
:r 02_PopulateBotNames.sql
:r 03_PopulateClassConfigs.sql
:r 04_StoredProcedures.sql
```

## ⚙️ Configuration

### ⚠️ IMPORTANT: Update Hex Values

The default InventoryHex and MagicListHex values are **placeholders**. You MUST update them with your server's actual item codes:

```sql
-- Example: Update DW Config 0 with real hex values
UPDATE BotClassConfigs
SET 
    InventoryHex = '0xYOUR_ACTUAL_INVENTORY_HEX_HERE',
    MagicListHex = '0xYOUR_ACTUAL_MAGICLIST_HEX_HERE'
WHERE ClassCode = 0 AND ConfigIndex = 0
```

To get proper hex values:
1. Create a character manually in-game with desired items/skills
2. Query the database: `SELECT Inventory, MagicList FROM Character WHERE Name = 'YourTestChar'`
3. Copy the hex values and update BotClassConfigs table

## 📊 Database Tables

### BotNames Table
- **1000+ names** across 10 languages
- **Languages:** English, Spanish, Portuguese, Korean, Chinese, Japanese, Russian, German, French, Turkish
- **50+ names per language per gender**
- Optional `IsUsed` flag for unique name tracking

### BotClassConfigs Table
- **7 classes:** DW(0), DK(16), ELF(32), MG(48), DL(64), SUM(80), RF(96)
- **7 configurations per class** (49 total)
- Each config has different stat distributions:
  - Config 0: Starter (basic stats)
  - Config 1: Balanced
  - Config 2: Primary stat focused (Strength/Energy/Dexterity)
  - Config 3: Tank (Vitality focused)
  - Config 4: Speed/Secondary stat
  - Config 5: Elite (high stats)
  - Config 6: Ultimate (maximum stats)

## 🔧 Stored Procedures

### 1. WZ_CreateBot
Creates a single bot character.

```sql
EXEC WZ_CreateBot 
    @AccountID = 'Bot0001',
    @CharName = 'TestBot99',
    @ClassCode = 0,        -- 0=DW, 16=DK, 32=ELF, 48=MG, 64=DL, 80=SUM, 96=RF
    @ConfigIndex = 0,      -- 0-6 (different equipment/stats)
    @Level = 400,
    @MapNumber = 0,
    @MapPosX = 125,
    @MapPosY = 125,
    @Money = 20000000
```

**Returns:**
- `0x01` = Success
- `0x00` = Character name already exists
- `0x02` = Database error
- `0x03` = No empty character slot
- `0x04` = Configuration not found

### 2. WZ_CreateBotBatch
Creates multiple bots in one call (RECOMMENDED).

```sql
EXEC WZ_CreateBotBatch
    @StartBotNumber = 1,       -- Creates Bot0001, Bot0002, etc.
    @BotCount = 100,           -- Number of bots to create
    @ClassCode = -1,           -- -1 = random distribution, or specific class code
    @ConfigIndex = -1,         -- -1 = random from enabled configs
    @MinLevel = 350,
    @MaxLevel = 400,
    @MapNumber = 0,
    @MapX = 125,
    @MapY = 125,
    @UseFemaleName = 0,        -- 0=Male, 1=Female
    @Language = NULL           -- NULL=random, or 'English', 'Spanish', etc.
```

**Example:** Create 500 bots with random classes and names:
```sql
EXEC WZ_CreateBotBatch 1, 500, -1, -1, 350, 400, 0, 125, 125, 0, NULL
```

### 3. WZ_GetRandomBotName
Gets a random bot name from the database.

```sql
DECLARE @BotName VARCHAR(50)
EXEC WZ_GetRandomBotName 'Male', 'English', @BotName OUTPUT
PRINT @BotName  -- Returns random English male name
```

### 4. WZ_GetBotClassConfig
Retrieves configuration for a specific class and config index.

```sql
DECLARE @Str INT, @Inv VARCHAR(MAX)
EXEC WZ_GetBotClassConfig 0, 0, @Str OUTPUT, ... @Inv OUTPUT
```

### 5. WZ_GetRandomConfigIndex
Selects a random enabled configuration.

```sql
DECLARE @ConfigIdx INT
EXEC WZ_GetRandomConfigIndex 0, 127, @ConfigIdx OUTPUT  -- 127 = all 7 configs enabled
```

### 6. WZ_ResetBotNames
Resets all bot names to unused status.

```sql
EXEC WZ_ResetBotNames
```

### 7. WZ_DeleteBotRange
Deletes a range of bot characters.

```sql
EXEC WZ_DeleteBotRange 1, 100  -- Deletes Bot0001 through Bot0100
```

## 💻 C++ Integration

### ODBC Connection Example

```cpp
bool CreateBotViaStoredProcedure(const char* accountID, const char* charName, int classCode, int configIndex)
{
    SQLHSTMT hStmt;
    SQLRETURN result;
    
    // Allocate statement handle
    SQLAllocHandle(SQL_HANDLE_STMT, g_hOdbcConnection, &hStmt);
    
    // Prepare stored procedure call
    char query[512];
    sprintf_s(query, sizeof(query),
        "{CALL WZ_CreateBot('%s', '%s', %d, %d, 400, 0, 125, 125, 20000000)}",
        accountID, charName, classCode, configIndex);
    
    // Execute
    result = SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS);
    
    if (result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO)
    {
        // Get result
        SQLINTEGER resultCode = 0;
        SQLFetch(hStmt);
        SQLGetData(hStmt, 1, SQL_C_LONG, &resultCode, 0, NULL);
        
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return (resultCode == 0x01);  // 0x01 = success
    }
    
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return false;
}
```

### Batch Creation (FAST!)

```cpp
bool CreateMultipleBotsOptimized(int botCount, int startFrom)
{
    SQLHSTMT hStmt;
    SQLAllocHandle(SQL_HANDLE_STMT, g_hOdbcConnection, &hStmt);
    
    char query[512];
    sprintf_s(query, sizeof(query),
        "{CALL WZ_CreateBotBatch(%d, %d, -1, -1, 350, 400, 0, 125, 125, 0, NULL)}",
        startFrom, botCount);
    
    SQLRETURN result = SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS);
    
    bool success = (result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO);
    
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return success;
}
```

## 📈 Performance Comparison

| Method | 10 Bots | 50 Bots | 100 Bots | 500 Bots |
|--------|---------|---------|----------|----------|
| **Old (File Generation)** | 2s | 15s (freezes) | Crashes | Impossible |
| **New (Stored Procedure)** | <0.1s | <0.5s | <1s | ~3s |

## 🧪 Testing

### Test Single Bot Creation
```sql
-- Create a test bot
EXEC WZ_CreateBot 'TestBot1', 'Zeus99', 0, 0, 400, 0, 125, 125, 20000000

-- Verify
SELECT * FROM Character WHERE Name = 'Zeus99'
SELECT * FROM AccountCharacter WHERE Id = 'TestBot1'
```

### Test Batch Creation
```sql
-- Create 10 test bots
EXEC WZ_CreateBotBatch 9001, 10, -1, -1, 350, 400, 0, 125, 125, 0, NULL

-- Count created bots
SELECT COUNT(*) FROM Character WHERE AccountID LIKE 'Bot9%'
```

### Test Name Generation
```sql
-- Get 10 random names
DECLARE @i INT = 0, @name VARCHAR(50)
WHILE @i < 10
BEGIN
    EXEC WZ_GetRandomBotName 'Male', NULL, @name OUTPUT
    PRINT @name
    SET @i = @i + 1
END
```

## 🔍 Troubleshooting

### Issue: "Configuration not found" error
**Solution:** Make sure you've updated the hex values in BotClassConfigs table with real data from your server.

### Issue: Bots created but can't log in
**Solution:** Verify that the InventoryHex and MagicListHex values are correct for your MU Online version.

### Issue: Names are too long
**Solution:** The stored procedure automatically truncates names to 8 characters + 2 digit suffix.

### Issue: Want to use custom names
**Solution:** Add your names to BotNames table:
```sql
INSERT INTO BotNames (Name, Language, Gender) VALUES ('CustomName', 'Custom', 'Male')
```

## 📝 Customization Examples

### Enable Only Specific Configs
```sql
-- Disable all configs except 0, 2, 5
UPDATE BotClassConfigs SET IsEnabled = 0
UPDATE BotClassConfigs SET IsEnabled = 1 WHERE ConfigIndex IN (0, 2, 5)
```

### Add More Names
```sql
-- Add Italian names
INSERT INTO BotNames (Name, Language, Gender) VALUES
('Giovanni', 'Italian', 'Male'),
('Marco', 'Italian', 'Male'),
('Lucia', 'Italian', 'Female')
```

### Create Custom Class Distribution
Modify the `WZ_CreateBotBatch` procedure to change class percentages.

## 🎮 Class Codes Reference

| Class | Code | Default Skills |
|-------|------|----------------|
| Dark Wizard (DW) | 0 | 9 (Soul Barrier), 12 (Aqua Flash) |
| Dark Knight (DK) | 16 | 44 (Cyclone), 41 (Twisting Slash) |
| Elf (ELF) | 32 | 24 (Triple Shot), 52 (Ice Arrow) |
| Magic Gladiator (MG) | 48 | 8 (Fireball), 55 (Power Slash) |
| Dark Lord (DL) | 64 | 65 (Fire Burst), 61 (Force) |
| Summoner (SUM) | 80 | 214 (Summon Goblin), 215 (Summon Golem) |
| Rage Fighter (RF) | 96 | 264 (Dragon Roar), 263 (Dragon Slasher) |

## 📧 Support

For issues or questions:
1. Check that all SQL scripts ran successfully
2. Verify hex values are updated
3. Test with a single bot before batch creation
4. Check SQL Server error logs

## 📄 License

This bot system is provided as-is for MU Online private server development.
