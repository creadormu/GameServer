-- =============================================
-- MuOnline Database Unicode Migration Script - FIXED VERSION
-- =============================================
-- This script converts varchar columns to nvarchar to support Unicode characters
-- for Portuguese, Chinese, Japanese, Korean, and other languages
--
-- FIXED: Handles primary key constraints properly
-- FIXED: Matches your actual table structure
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
-- FIXED: Drop and recreate primary key constraint
-- =============================================
PRINT 'Migrating AccountCharacter table...'

-- Drop primary key constraint first
IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_AccountCharacter' AND object_id = OBJECT_ID('AccountCharacter'))
BEGIN
    ALTER TABLE [dbo].[AccountCharacter] DROP CONSTRAINT [PK_AccountCharacter]
    PRINT '  Dropped PK_AccountCharacter constraint'
END

-- Now we can alter the columns
ALTER TABLE [dbo].[AccountCharacter] ALTER COLUMN [Id] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[AccountCharacter] ALTER COLUMN [GameID1] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter] ALTER COLUMN [GameID2] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter] ALTER COLUMN [GameID3] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter] ALTER COLUMN [GameID4] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter] ALTER COLUMN [GameID5] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter] ALTER COLUMN [GameID6] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter] ALTER COLUMN [GameIDC] NVARCHAR(10) NULL

-- Recreate primary key constraint
ALTER TABLE [dbo].[AccountCharacter] 
ADD CONSTRAINT [PK_AccountCharacter] PRIMARY KEY NONCLUSTERED ([Id] ASC)
WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, 
      ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]

PRINT '  AccountCharacter: Converted varchar to nvarchar for character names'
PRINT ''

-- =============================================
-- 3. Character Table - Main Character Data
-- FIXED: Drop and recreate primary key constraint
-- =============================================
PRINT 'Migrating Character table...'

-- Drop primary key constraint first
IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_Character' AND object_id = OBJECT_ID('Character'))
BEGIN
    ALTER TABLE [dbo].[Character] DROP CONSTRAINT [PK_Character]
    PRINT '  Dropped PK_Character constraint'
END

-- Now we can alter the columns
ALTER TABLE [dbo].[Character] ALTER COLUMN [AccountID] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[Character] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[Character] ALTER COLUMN [Wife] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[Character] ALTER COLUMN [Language] NVARCHAR(20) NULL

-- Recreate primary key constraint
ALTER TABLE [dbo].[Character] 
ADD CONSTRAINT [PK_Character] PRIMARY KEY NONCLUSTERED ([Name] ASC)
WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, 
      ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]

PRINT '  Character: Converted varchar to nvarchar for text fields'
PRINT ''

-- =============================================
-- 4. Guild Table
-- FIXED: Uses 'Number' column (not G_Num)
-- =============================================
PRINT 'Migrating Guild table...'

-- Drop primary key constraint first
IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_Guild' AND object_id = OBJECT_ID('Guild'))
BEGIN
    ALTER TABLE [dbo].[Guild] DROP CONSTRAINT [PK_Guild]
    PRINT '  Dropped PK_Guild constraint'
END

-- Alter columns to nvarchar
ALTER TABLE [dbo].[Guild] ALTER COLUMN [G_Name] NVARCHAR(8) NOT NULL
ALTER TABLE [dbo].[Guild] ALTER COLUMN [G_Master] NVARCHAR(10) NULL
ALTER TABLE [dbo].[Guild] ALTER COLUMN [G_Notice] NVARCHAR(60) NULL

-- Recreate primary key constraint
ALTER TABLE [dbo].[Guild] 
ADD CONSTRAINT [PK_Guild] PRIMARY KEY CLUSTERED ([G_Name] ASC)
WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, 
      ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]

PRINT '  Guild: Converted varchar to nvarchar for text fields'
PRINT ''

-- =============================================
-- 5. GuildMember Table
-- FIXED: Drop and recreate primary key constraint
-- =============================================
PRINT 'Migrating GuildMember table...'

-- Drop primary key constraint first
IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_GuildMember' AND object_id = OBJECT_ID('GuildMember'))
BEGIN
    ALTER TABLE [dbo].[GuildMember] DROP CONSTRAINT [PK_GuildMember]
    PRINT '  Dropped PK_GuildMember constraint'
END

-- Alter columns to nvarchar
ALTER TABLE [dbo].[GuildMember] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[GuildMember] ALTER COLUMN [G_Name] NVARCHAR(8) NOT NULL

-- Recreate primary key constraint
ALTER TABLE [dbo].[GuildMember] 
ADD CONSTRAINT [PK_GuildMember] PRIMARY KEY CLUSTERED ([Name] ASC)
WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, 
      ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]

PRINT '  GuildMember: Converted varchar to nvarchar for character names'
PRINT ''

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

PRINT ''
PRINT '========================================='
PRINT 'Unicode Migration Completed Successfully!'
PRINT '========================================='
PRINT ''
PRINT 'Next Steps:'
PRINT '1. Update GameServer.exe with Unicode-enabled version'
PRINT '2. Update stored procedures (run Database_StoredProcedure_Unicode_Update_FIXED.sql)'
PRINT '3. Test character creation, login, and chat with Unicode characters'
PRINT ''

-- =============================================
-- Verify Migration
-- =============================================
PRINT 'Verification Query:'
PRINT 'Showing converted columns:'
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

PRINT ''
PRINT 'All varchar columns should now show as nvarchar!'

GO
