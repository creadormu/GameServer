USE [MuOnline]
GO

-- =============================================
-- Bot Management Tables
-- =============================================

-- Table for storing bot names in multiple languages
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[BotNames]') AND type in (N'U'))
BEGIN
    CREATE TABLE [dbo].[BotNames]
    (
        [NameID] INT IDENTITY(1,1) PRIMARY KEY,
        [Name] VARCHAR(50) NOT NULL,
        [Language] VARCHAR(20) NOT NULL,  -- English, Spanish, Portuguese, Korean, Chinese, Japanese, Russian, German, French, Turkish
        [Gender] VARCHAR(10) NOT NULL,    -- Male, Female
        [IsUsed] BIT DEFAULT 0,
        CONSTRAINT UQ_BotNames UNIQUE (Name)
    )
    PRINT 'Table BotNames created successfully'
END
GO

-- Table for storing class configurations
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[BotClassConfigs]') AND type in (N'U'))
BEGIN
    CREATE TABLE [dbo].[BotClassConfigs]
    (
        [ConfigID] INT IDENTITY(1,1) PRIMARY KEY,
        [ClassCode] INT NOT NULL,          -- 0=DW, 16=DK, 32=ELF, 48=MG, 64=DL, 80=SUM, 96=RF
        [ConfigIndex] INT NOT NULL,        -- 0-6 (7 configs per class)
        [ConfigName] VARCHAR(50) NOT NULL,
        [Strength] INT NOT NULL,
        [Dexterity] INT NOT NULL,
        [Vitality] INT NOT NULL,
        [Energy] INT NOT NULL,
        [Leadership] INT NOT NULL,
        [InventoryHex] VARCHAR(MAX) NOT NULL,
        [MagicListHex] VARCHAR(MAX) NOT NULL,
        [MainSkill] INT NOT NULL,
        [SecondarySkill] INT NOT NULL,
        [Buff1] INT DEFAULT -1,
        [Buff2] INT DEFAULT -1,
        [Buff3] INT DEFAULT -1,
        [IsEnabled] BIT DEFAULT 1,
        CONSTRAINT UQ_ClassConfig UNIQUE (ClassCode, ConfigIndex)
    )
    PRINT 'Table BotClassConfigs created successfully'
END
GO

-- Index for faster lookups
IF NOT EXISTS (SELECT * FROM sys.indexes WHERE name = 'IX_BotClassConfigs_Class_Enabled')
BEGIN
    CREATE INDEX IX_BotClassConfigs_Class_Enabled 
    ON [dbo].[BotClassConfigs] ([ClassCode], [IsEnabled])
END
GO

IF NOT EXISTS (SELECT * FROM sys.indexes WHERE name = 'IX_BotNames_Language_Gender')
BEGIN
    CREATE INDEX IX_BotNames_Language_Gender 
    ON [dbo].[BotNames] ([Language], [Gender], [IsUsed])
END
GO

PRINT 'All tables and indexes created successfully'
GO
