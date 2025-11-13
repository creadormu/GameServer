-- =============================================
-- MuOnline Database Unicode Migration Script
-- =============================================
-- This script converts varchar columns to nvarchar to support Unicode characters
-- for Portuguese, Chinese, Japanese, Korean, and other languages
--
-- IMPORTANT: BACKUP YOUR DATABASE BEFORE RUNNING THIS SCRIPT!
-- 
-- Run this on your MuOnline database
-- =============================================

USE [MuOnline]
GO

PRINT '========================================='
PRINT 'Starting Unicode Migration for MuOnline'
PRINT '========================================='
PRINT ''

-- =============================================
-- 1. MEMB_INFO Table - Account Information
-- =============================================
PRINT 'Migrating MEMB_INFO table...'

-- Drop dependent objects if they exist (foreign keys, constraints, etc.)
-- Note: Adjust these based on your actual database schema

ALTER TABLE [dbo].[MEMB_INFO] ALTER COLUMN [memb___id] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[MEMB_INFO] ALTER COLUMN [memb__pwd] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[MEMB_INFO] ALTER COLUMN [memb_name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[MEMB_INFO] ALTER COLUMN [addr_info] NVARCHAR(50) NULL
ALTER TABLE [dbo].[MEMB_INFO] ALTER COLUMN [addr_deta] NVARCHAR(50) NULL
ALTER TABLE [dbo].[MEMB_INFO] ALTER COLUMN [tel__numb] NVARCHAR(20) NULL
ALTER TABLE [dbo].[MEMB_INFO] ALTER COLUMN [phon_numb] NVARCHAR(15) NULL
ALTER TABLE [dbo].[MEMB_INFO] ALTER COLUMN [mail_addr] NVARCHAR(50) NULL
ALTER TABLE [dbo].[MEMB_INFO] ALTER COLUMN [fpas_ques] NVARCHAR(50) NULL
ALTER TABLE [dbo].[MEMB_INFO] ALTER COLUMN [fpas_answ] NVARCHAR(50) NULL

PRINT '  MEMB_INFO: Converted varchar to nvarchar for text fields'
PRINT ''

-- =============================================
-- 2. AccountCharacter Table - Character List
-- =============================================
PRINT 'Migrating AccountCharacter table...'

ALTER TABLE [dbo].[AccountCharacter] ALTER COLUMN [Id] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[AccountCharacter] ALTER COLUMN [GameID1] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter] ALTER COLUMN [GameID2] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter] ALTER COLUMN [GameID3] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter] ALTER COLUMN [GameID4] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter] ALTER COLUMN [GameID5] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter] ALTER COLUMN [GameID6] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter] ALTER COLUMN [GameIDC] NVARCHAR(10) NULL

PRINT '  AccountCharacter: Converted varchar to nvarchar for character names'
PRINT ''

-- =============================================
-- 3. Character Table - Main Character Data
-- =============================================
PRINT 'Migrating Character table...'

ALTER TABLE [dbo].[Character] ALTER COLUMN [AccountID] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[Character] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[Character] ALTER COLUMN [Wife] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[Character] ALTER COLUMN [Language] NVARCHAR(20) NULL

PRINT '  Character: Converted varchar to nvarchar for text fields'
PRINT ''

-- =============================================
-- 4. Guild Table (if exists)
-- =============================================
IF EXISTS (SELECT * FROM sys.tables WHERE name = 'Guild')
BEGIN
    PRINT 'Migrating Guild table...'
    
    -- Check if columns exist before altering
    IF EXISTS (SELECT * FROM sys.columns WHERE object_id = OBJECT_ID('dbo.Guild') AND name = 'G_Name')
        ALTER TABLE [dbo].[Guild] ALTER COLUMN [G_Name] NVARCHAR(10) NULL
    
    IF EXISTS (SELECT * FROM sys.columns WHERE object_id = OBJECT_ID('dbo.Guild') AND name = 'G_Master')
        ALTER TABLE [dbo].[Guild] ALTER COLUMN [G_Master] NVARCHAR(10) NULL
    
    IF EXISTS (SELECT * FROM sys.columns WHERE object_id = OBJECT_ID('dbo.Guild') AND name = 'G_Notice')
        ALTER TABLE [dbo].[Guild] ALTER COLUMN [G_Notice] NVARCHAR(60) NULL
    
    PRINT '  Guild: Converted varchar to nvarchar for text fields'
    PRINT ''
END

-- =============================================
-- 5. GuildMember Table (if exists)
-- =============================================
IF EXISTS (SELECT * FROM sys.tables WHERE name = 'GuildMember')
BEGIN
    PRINT 'Migrating GuildMember table...'
    
    IF EXISTS (SELECT * FROM sys.columns WHERE object_id = OBJECT_ID('dbo.GuildMember') AND name = 'Name')
        ALTER TABLE [dbo].[GuildMember] ALTER COLUMN [Name] NVARCHAR(10) NULL
    
    PRINT '  GuildMember: Converted varchar to nvarchar for character names'
    PRINT ''
END

-- =============================================
-- 6. ChatLog Table (if exists) - IMPORTANT FOR CHAT HISTORY
-- =============================================
IF EXISTS (SELECT * FROM sys.tables WHERE name = 'ChatLog')
BEGIN
    PRINT 'Migrating ChatLog table...'
    
    IF EXISTS (SELECT * FROM sys.columns WHERE object_id = OBJECT_ID('dbo.ChatLog') AND name = 'CharacterName')
        ALTER TABLE [dbo].[ChatLog] ALTER COLUMN [CharacterName] NVARCHAR(10) NULL
    
    IF EXISTS (SELECT * FROM sys.columns WHERE object_id = OBJECT_ID('dbo.ChatLog') AND name = 'Message')
        ALTER TABLE [dbo].[ChatLog] ALTER COLUMN [Message] NVARCHAR(255) NULL
    
    PRINT '  ChatLog: Converted varchar to nvarchar for chat messages'
    PRINT ''
END

-- =============================================
-- 7. Mail/Message Tables (if exists)
-- =============================================
IF EXISTS (SELECT * FROM sys.tables WHERE name = 'MemoData')
BEGIN
    PRINT 'Migrating MemoData table...'
    
    IF EXISTS (SELECT * FROM sys.columns WHERE object_id = OBJECT_ID('dbo.MemoData') AND name = 'Name')
        ALTER TABLE [dbo].[MemoData] ALTER COLUMN [Name] NVARCHAR(10) NULL
    
    IF EXISTS (SELECT * FROM sys.columns WHERE object_id = OBJECT_ID('dbo.MemoData') AND name = 'Subject')
        ALTER TABLE [dbo].[MemoData] ALTER COLUMN [Subject] NVARCHAR(60) NULL
    
    IF EXISTS (SELECT * FROM sys.columns WHERE object_id = OBJECT_ID('dbo.MemoData') AND name = 'Memo')
        ALTER TABLE [dbo].[MemoData] ALTER COLUMN [Memo] NVARCHAR(1000) NULL
    
    PRINT '  MemoData: Converted varchar to nvarchar for messages'
    PRINT ''
END

-- =============================================
-- 8. Personal Shop Tables (if exists)
-- =============================================
IF EXISTS (SELECT * FROM sys.tables WHERE name = 'PersonalShop')
BEGIN
    PRINT 'Migrating PersonalShop table...'
    
    IF EXISTS (SELECT * FROM sys.columns WHERE object_id = OBJECT_ID('dbo.PersonalShop') AND name = 'ShopName')
        ALTER TABLE [dbo].[PersonalShop] ALTER COLUMN [ShopName] NVARCHAR(36) NULL
    
    IF EXISTS (SELECT * FROM sys.columns WHERE object_id = OBJECT_ID('dbo.PersonalShop') AND name = 'OwnerName')
        ALTER TABLE [dbo].[PersonalShop] ALTER COLUMN [OwnerName] NVARCHAR(10) NULL
    
    PRINT '  PersonalShop: Converted varchar to nvarchar for shop names'
    PRINT ''
END

-- =============================================
-- 9. Custom Tables - Add your custom tables here
-- =============================================
-- Example for custom ranking tables:
/*
IF EXISTS (SELECT * FROM sys.tables WHERE name = 'YourCustomTable')
BEGIN
    PRINT 'Migrating YourCustomTable...'
    
    ALTER TABLE [dbo].[YourCustomTable] ALTER COLUMN [PlayerName] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[YourCustomTable] ALTER COLUMN [CustomText] NVARCHAR(50) NULL
    
    PRINT '  YourCustomTable: Converted varchar to nvarchar'
    PRINT ''
END
*/

-- =============================================
-- 10. Rebuild Indexes (Optional but Recommended)
-- =============================================
PRINT 'Rebuilding indexes...'

-- AccountCharacter indexes
IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_AccountCharacter' AND object_id = OBJECT_ID('AccountCharacter'))
BEGIN
    ALTER INDEX [PK_AccountCharacter] ON [dbo].[AccountCharacter] REBUILD
    PRINT '  Rebuilt PK_AccountCharacter index'
END

-- Character indexes
IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_Character' AND object_id = OBJECT_ID('Character'))
BEGIN
    ALTER INDEX [PK_Character] ON [dbo].[Character] REBUILD
    PRINT '  Rebuilt PK_Character index'
END

-- MEMB_INFO indexes
IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_MEMB_INFO_1' AND object_id = OBJECT_ID('MEMB_INFO'))
BEGIN
    ALTER INDEX [PK_MEMB_INFO_1] ON [dbo].[MEMB_INFO] REBUILD
    PRINT '  Rebuilt PK_MEMB_INFO_1 index'
END

PRINT ''
PRINT '========================================='
PRINT 'Unicode Migration Completed Successfully!'
PRINT '========================================='
PRINT ''
PRINT 'Next Steps:'
PRINT '1. Update GameServer.exe with Unicode-enabled version'
PRINT '2. Update any stored procedures that use varchar parameters to nvarchar'
PRINT '3. Test character creation, login, and chat with Unicode characters'
PRINT '4. Configure database collation to support Unicode if needed'
PRINT ''
PRINT 'Recommended Database Collation: SQL_Latin1_General_CP1_CI_AS or Latin1_General_CI_AS'
PRINT ''

-- =============================================
-- Verify Migration
-- =============================================
PRINT 'Verification Query:'
PRINT 'SELECT TABLE_NAME, COLUMN_NAME, DATA_TYPE, CHARACTER_MAXIMUM_LENGTH'
PRINT 'FROM INFORMATION_SCHEMA.COLUMNS'
PRINT 'WHERE TABLE_SCHEMA = ''dbo'' AND DATA_TYPE IN (''varchar'', ''nvarchar'')'
PRINT 'ORDER BY TABLE_NAME, COLUMN_NAME'
PRINT ''

SELECT 
    TABLE_NAME, 
    COLUMN_NAME, 
    DATA_TYPE, 
    CHARACTER_MAXIMUM_LENGTH
FROM INFORMATION_SCHEMA.COLUMNS
WHERE TABLE_SCHEMA = 'dbo' 
    AND DATA_TYPE IN ('varchar', 'nvarchar')
    AND TABLE_NAME IN ('MEMB_INFO', 'AccountCharacter', 'Character', 'Guild', 'GuildMember')
ORDER BY TABLE_NAME, COLUMN_NAME

GO
