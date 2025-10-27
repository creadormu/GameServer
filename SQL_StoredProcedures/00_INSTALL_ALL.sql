-- =============================================
-- MU Online Bot System - Complete Installation Script
-- =============================================
-- This script will install all required tables, populate data,
-- and create stored procedures for bot management
-- 
-- Installation Order:
-- 1. Create tables for bot names and class configurations
-- 2. Populate bot names (10 languages × 2 genders × 50+ names)
-- 3. Populate class configurations (7 classes × 7 configs)
-- 4. Create all stored procedures
--
-- Usage:
-- Run this script in SQL Server Management Studio
-- or execute via sqlcmd:
-- sqlcmd -S localhost -d MuOnline -i 00_INSTALL_ALL.sql
-- =============================================

USE [MuOnline]
GO

PRINT '=========================================='
PRINT 'MU Online Bot System Installation'
PRINT 'Starting installation...'
PRINT '=========================================='
GO

-- Step 1: Create Tables
PRINT ''
PRINT 'STEP 1: Creating tables...'
GO

:r 01_CreateTables.sql

-- Step 2: Populate Bot Names
PRINT ''
PRINT 'STEP 2: Populating bot names...'
GO

:r 02_PopulateBotNames.sql

-- Step 3: Populate Class Configurations
PRINT ''
PRINT 'STEP 3: Populating class configurations...'
PRINT 'NOTE: Default hex values are placeholders.'
PRINT 'You MUST update InventoryHex and MagicListHex values'
PRINT 'in the BotClassConfigs table with your server-specific'
PRINT 'item codes and skill configurations!'
GO

:r 03_PopulateClassConfigs.sql

-- Step 4: Create Stored Procedures
PRINT ''
PRINT 'STEP 4: Creating stored procedures...'
GO

:r 04_StoredProcedures.sql

-- Verification
PRINT ''
PRINT '=========================================='
PRINT 'Installation Complete!'
PRINT '=========================================='
PRINT ''
PRINT 'Verification:'
PRINT '-------------'

SELECT 
    'BotNames' AS TableName,
    COUNT(*) AS RecordCount,
    COUNT(DISTINCT Language) AS LanguageCount,
    COUNT(DISTINCT Gender) AS GenderCount
FROM BotNames

SELECT 
    'BotClassConfigs' AS TableName,
    COUNT(*) AS RecordCount,
    COUNT(DISTINCT ClassCode) AS ClassCount,
    COUNT(DISTINCT ConfigIndex) AS ConfigsPerClass
FROM BotClassConfigs

PRINT ''
PRINT 'Stored Procedures Created:'
PRINT '  - WZ_GetRandomBotName'
PRINT '  - WZ_GetBotClassConfig'
PRINT '  - WZ_GetRandomConfigIndex'
PRINT '  - WZ_CreateBot'
PRINT '  - WZ_CreateBotBatch'
PRINT '  - WZ_ResetBotNames'
PRINT '  - WZ_DeleteBotRange'
PRINT ''
PRINT '=========================================='
PRINT 'IMPORTANT: NEXT STEPS'
PRINT '=========================================='
PRINT '1. Update BotClassConfigs table with proper'
PRINT '   InventoryHex and MagicListHex values for'
PRINT '   your server configuration'
PRINT ''
PRINT '2. Test bot creation with:'
PRINT '   EXEC WZ_CreateBot ''Bot0001'', ''TestBot99'', 0, 0, 400, 0, 125, 125, 20000000'
PRINT ''
PRINT '3. Create multiple bots with:'
PRINT '   EXEC WZ_CreateBotBatch 1, 10, -1, -1, 350, 400, 0, 125, 125, 0, NULL'
PRINT ''
PRINT '4. Update your GameServer C++ code to call'
PRINT '   WZ_CreateBot stored procedure via ODBC'
PRINT '=========================================='
GO
