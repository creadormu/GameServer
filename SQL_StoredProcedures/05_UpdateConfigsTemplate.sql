USE [MuOnline]
GO

-- =============================================
-- Template: Update Bot Class Configurations
-- =============================================
-- IMPORTANT: Replace the placeholder hex values below with
-- real values from your MU Online database.
--
-- HOW TO GET REAL HEX VALUES:
-- 1. Create a character in-game with desired items/skills
-- 2. Query: SELECT Inventory, MagicList FROM Character WHERE Name = 'YourTestChar'
-- 3. Copy the hex values (they will be like 0xFFFFFFFF...)
-- 4. Replace the values below
-- =============================================

-- =============================================
-- DARK WIZARD (Class 0) - Update Configs
-- =============================================

-- DW Config 0: Starter
UPDATE BotClassConfigs
SET 
    InventoryHex = '0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF',
    MagicListHex = '0xFF01FF020009FF0CFF10FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
WHERE ClassCode = 0 AND ConfigIndex = 0
GO

-- DW Config 1: Balanced
UPDATE BotClassConfigs
SET 
    InventoryHex = '0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF',
    MagicListHex = '0xFF01FF020009FF0CFF10FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
WHERE ClassCode = 0 AND ConfigIndex = 1
GO

-- Add more configs as needed...

-- =============================================
-- DARK KNIGHT (Class 16) - Update Configs
-- =============================================

-- DK Config 0: Starter
UPDATE BotClassConfigs
SET 
    InventoryHex = '0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF',
    MagicListHex = '0xFF01FF022CFF29FF30FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
WHERE ClassCode = 16 AND ConfigIndex = 0
GO

-- =============================================
-- QUICK UPDATE: Same hex for all configs of a class
-- =============================================
-- Use this if you want all configs of a class to have the same items
-- Just update the stats variation

-- Example: Update all DW configs with same hex
/*
UPDATE BotClassConfigs
SET 
    InventoryHex = '0xYOUR_DW_INVENTORY_HEX_HERE',
    MagicListHex = '0xYOUR_DW_MAGICLIST_HEX_HERE'
WHERE ClassCode = 0
GO
*/

-- =============================================
-- VERIFY YOUR UPDATES
-- =============================================
-- Run this to see what you've configured

SELECT 
    ClassCode,
    ConfigIndex,
    ConfigName,
    Strength, Dexterity, Vitality, Energy, Leadership,
    CASE 
        WHEN LEN(InventoryHex) > 100 THEN 'CONFIGURED'
        ELSE 'DEFAULT (NEEDS UPDATE)'
    END AS InventoryStatus,
    CASE 
        WHEN LEN(MagicListHex) > 100 THEN 'CONFIGURED'
        ELSE 'DEFAULT (NEEDS UPDATE)'
    END AS MagicListStatus,
    IsEnabled
FROM BotClassConfigs
ORDER BY ClassCode, ConfigIndex
GO

-- =============================================
-- EXPORT EXISTING CHARACTER FOR REFERENCE
-- =============================================
-- Use this to get hex values from an existing character

-- Example: Export a DW character's data
/*
SELECT 
    Name,
    Class,
    cLevel,
    Strength, Dexterity, Vitality, Energy, Leadership,
    CONVERT(VARCHAR(MAX), Inventory, 1) AS InventoryHex,
    CONVERT(VARCHAR(MAX), MagicList, 1) AS MagicListHex
FROM Character
WHERE Name = 'YourDWCharacter'
GO
*/

PRINT 'Configuration update template loaded'
PRINT 'Remember to replace placeholder hex values with real data!'
GO
