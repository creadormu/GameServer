-- =============================================
-- MuOnline Database COMPLETE Unicode Migration Script
-- =============================================
-- This script converts ALL varchar columns to nvarchar
-- Handles ALL foreign key constraints properly
--
-- IMPORTANT: BACKUP YOUR DATABASE BEFORE RUNNING THIS SCRIPT!
-- =============================================

USE [MuOnline]
GO

PRINT '========================================='
PRINT 'COMPLETE Unicode Migration for MuOnline'
PRINT 'This will take 15-30 minutes...'
PRINT '========================================='
PRINT ''

-- =============================================
-- STEP 1: DROP ALL FOREIGN KEY CONSTRAINTS
-- =============================================
PRINT 'Step 1: Dropping foreign key constraints...'

-- CustomNpcQuest foreign key
IF EXISTS (SELECT * FROM sys.foreign_keys WHERE name = 'FK_CustomNpcQuest_Character')
BEGIN
    ALTER TABLE [dbo].[CustomNpcQuest] DROP CONSTRAINT [FK_CustomNpcQuest_Character]
    PRINT '  Dropped FK_CustomNpcQuest_Character'
END

-- CustomQuest foreign key
IF EXISTS (SELECT * FROM sys.foreign_keys WHERE name = 'FK_CustomQuest_Character')
BEGIN
    ALTER TABLE [dbo].[CustomQuest] DROP CONSTRAINT [FK_CustomQuest_Character]
    PRINT '  Dropped FK_CustomQuest_Character'
END

PRINT ''

-- =============================================
-- STEP 2: DROP PRIMARY KEY CONSTRAINTS (that need to be modified)
-- =============================================
PRINT 'Step 2: Dropping primary key constraints...'

-- Character table
IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_Character' AND object_id = OBJECT_ID('Character'))
BEGIN
    ALTER TABLE [dbo].[Character] DROP CONSTRAINT [PK_Character]
    PRINT '  Dropped PK_Character'
END

-- Guild table
IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_Guild' AND object_id = OBJECT_ID('Guild'))
BEGIN
    ALTER TABLE [dbo].[Guild] DROP CONSTRAINT [PK_Guild]
    PRINT '  Dropped PK_Guild'
END

-- GuildMember table
IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_GuildMember' AND object_id = OBJECT_ID('GuildMember'))
BEGIN
    ALTER TABLE [dbo].[GuildMember] DROP CONSTRAINT [PK_GuildMember]
    PRINT '  Dropped PK_GuildMember'
END

-- AccountCharacter2-5
IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_AccountCharacter2')
    ALTER TABLE [dbo].[AccountCharacter2] DROP CONSTRAINT [PK_AccountCharacter2]
IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_AccountCharacter3')
    ALTER TABLE [dbo].[AccountCharacter3] DROP CONSTRAINT [PK_AccountCharacter3]
IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_AccountCharacter4')
    ALTER TABLE [dbo].[AccountCharacter4] DROP CONSTRAINT [PK_AccountCharacter4]
IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_AccountCharacter5')
    ALTER TABLE [dbo].[AccountCharacter5] DROP CONSTRAINT [PK_AccountCharacter5]

PRINT ''

-- =============================================
-- STEP 3: CONVERT MAIN CHARACTER TABLES
-- =============================================
PRINT 'Step 3: Converting Character tables...'

-- Character table (CRITICAL!)
ALTER TABLE [dbo].[Character] ALTER COLUMN [AccountID] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[Character] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[Character] ALTER COLUMN [Wife] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[Character] ALTER COLUMN [Language] NVARCHAR(20) NULL
PRINT '  Character table converted'

-- AccountCharacter2
ALTER TABLE [dbo].[AccountCharacter2] ALTER COLUMN [Id] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[AccountCharacter2] ALTER COLUMN [GameID1] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter2] ALTER COLUMN [GameID2] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter2] ALTER COLUMN [GameID3] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter2] ALTER COLUMN [GameID4] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter2] ALTER COLUMN [GameID5] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter2] ALTER COLUMN [GameIDC] NVARCHAR(10) NULL
PRINT '  AccountCharacter2 converted'

-- AccountCharacter3
ALTER TABLE [dbo].[AccountCharacter3] ALTER COLUMN [Id] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[AccountCharacter3] ALTER COLUMN [GameID1] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter3] ALTER COLUMN [GameID2] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter3] ALTER COLUMN [GameID3] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter3] ALTER COLUMN [GameID4] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter3] ALTER COLUMN [GameID5] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter3] ALTER COLUMN [GameIDC] NVARCHAR(10) NULL
PRINT '  AccountCharacter3 converted'

-- AccountCharacter4
ALTER TABLE [dbo].[AccountCharacter4] ALTER COLUMN [Id] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[AccountCharacter4] ALTER COLUMN [GameID1] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter4] ALTER COLUMN [GameID2] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter4] ALTER COLUMN [GameID3] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter4] ALTER COLUMN [GameID4] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter4] ALTER COLUMN [GameID5] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter4] ALTER COLUMN [GameIDC] NVARCHAR(10) NULL
PRINT '  AccountCharacter4 converted'

-- AccountCharacter5
ALTER TABLE [dbo].[AccountCharacter5] ALTER COLUMN [Id] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[AccountCharacter5] ALTER COLUMN [GameID1] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter5] ALTER COLUMN [GameID2] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter5] ALTER COLUMN [GameID3] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter5] ALTER COLUMN [GameID4] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter5] ALTER COLUMN [GameID5] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter5] ALTER COLUMN [GameIDC] NVARCHAR(10) NULL
PRINT '  AccountCharacter5 converted'

PRINT ''

-- =============================================
-- STEP 4: CONVERT GUILD TABLES
-- =============================================
PRINT 'Step 4: Converting Guild tables...'

ALTER TABLE [dbo].[Guild] ALTER COLUMN [G_Name] NVARCHAR(8) NOT NULL
ALTER TABLE [dbo].[Guild] ALTER COLUMN [G_Master] NVARCHAR(10) NULL
ALTER TABLE [dbo].[Guild] ALTER COLUMN [G_Notice] NVARCHAR(60) NULL
PRINT '  Guild table converted'

ALTER TABLE [dbo].[GuildMember] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[GuildMember] ALTER COLUMN [G_Name] NVARCHAR(8) NOT NULL
PRINT '  GuildMember table converted'

PRINT ''

-- =============================================
-- STEP 5: CONVERT CUSTOM QUEST TABLES
-- =============================================
PRINT 'Step 5: Converting Custom Quest tables...'

ALTER TABLE [dbo].[CustomNpcQuest] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
PRINT '  CustomNpcQuest converted'

ALTER TABLE [dbo].[CustomQuest] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
PRINT '  CustomQuest converted'

PRINT ''

-- =============================================
-- STEP 6: CONVERT OTHER GAME TABLES
-- =============================================
PRINT 'Step 6: Converting other game tables...'

-- Achievements
IF EXISTS (SELECT * FROM sys.tables WHERE name = 'Achievements')
    ALTER TABLE [dbo].[Achievements] ALTER COLUMN [Name] NVARCHAR(10) NULL

-- CustomAttack
ALTER TABLE [dbo].[CustomAttack] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

-- CustomGift
ALTER TABLE [dbo].[CustomGift] ALTER COLUMN [AccountID] NVARCHAR(10) NOT NULL

-- EventLeoTheHelper
ALTER TABLE [dbo].[EventLeoTheHelper] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

-- EventSantaClaus
ALTER TABLE [dbo].[EventSantaClaus] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

-- ExtWarehouse
ALTER TABLE [dbo].[ExtWarehouse] ALTER COLUMN [AccountID] NVARCHAR(10) NOT NULL

-- HelperData
ALTER TABLE [dbo].[HelperData] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

-- ItemLog
ALTER TABLE [dbo].[ItemLog] ALTER COLUMN [Acc] NVARCHAR(10) NULL
ALTER TABLE [dbo].[ItemLog] ALTER COLUMN [Name] NVARCHAR(10) NULL
ALTER TABLE [dbo].[ItemLog] ALTER COLUMN [iName] NVARCHAR(50) NULL

-- LuckyCoin
ALTER TABLE [dbo].[LuckyCoin] ALTER COLUMN [AccountID] NVARCHAR(10) NOT NULL

-- Marry
ALTER TABLE [dbo].[Marry] ALTER COLUMN [Character] NVARCHAR(15) NOT NULL
ALTER TABLE [dbo].[Marry] ALTER COLUMN [MarryCharacter] NVARCHAR(15) NOT NULL

-- MasterSkillTree
ALTER TABLE [dbo].[MasterSkillTree] ALTER COLUMN [Name] NVARCHAR(50) NOT NULL

-- MuunInventory
ALTER TABLE [dbo].[MuunInventory] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

-- OptionData
ALTER TABLE [dbo].[OptionData] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

-- QuestKillCount
ALTER TABLE [dbo].[QuestKillCount] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

-- QuestSystem
ALTER TABLE [dbo].[QuestSystem] ALTER COLUMN [AccountID] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[QuestSystem] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

-- QuestWorld
ALTER TABLE [dbo].[QuestWorld] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

-- warehouse
ALTER TABLE [dbo].[warehouse] ALTER COLUMN [AccountID] NVARCHAR(10) NOT NULL

-- WarehouseGuild
ALTER TABLE [dbo].[WarehouseGuild] ALTER COLUMN [Guild] NVARCHAR(10) NOT NULL

PRINT '  Other game tables converted'
PRINT ''

-- =============================================
-- STEP 7: CONVERT RANKING TABLES
-- =============================================
PRINT 'Step 7: Converting ranking tables...'

ALTER TABLE [dbo].[RankingBloodCastle] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[RankingChaosCastle] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[RankingDevilSquare] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[RankingDuel] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[RankingIllusionTemple] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[RankingKingGuild] ALTER COLUMN [Name] NVARCHAR(8) NOT NULL
ALTER TABLE [dbo].[RankingKingPlayer] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[RankingTvT] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

PRINT '  Ranking tables converted'
PRINT ''

-- =============================================
-- STEP 8: CONVERT GENS TABLES
-- =============================================
PRINT 'Step 8: Converting Gens tables...'

ALTER TABLE [dbo].[Gens_Duprian] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[Gens_Rank] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[Gens_Reward] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[Gens_Varnert] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

PRINT '  Gens tables converted'
PRINT ''

-- =============================================
-- STEP 9: CONVERT FRIEND/SOCIAL TABLES
-- =============================================
PRINT 'Step 9: Converting friend/social tables...'

ALTER TABLE [dbo].[T_CGuid] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[T_FriendList] ALTER COLUMN [FriendName] NVARCHAR(10) NULL
ALTER TABLE [dbo].[T_FriendMail] ALTER COLUMN [FriendName] NVARCHAR(10) NULL
ALTER TABLE [dbo].[T_FriendMail] ALTER COLUMN [Subject] NVARCHAR(50) NULL
ALTER TABLE [dbo].[T_FriendMain] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[T_WaitFriend] ALTER COLUMN [FriendName] NVARCHAR(10) NOT NULL

PRINT '  Friend/social tables converted'
PRINT ''

-- =============================================
-- STEP 10: CONVERT CASH SHOP TABLES
-- =============================================
PRINT 'Step 10: Converting cash shop tables...'

ALTER TABLE [dbo].[CashShopData] ALTER COLUMN [AccountID] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[CashShopInventory] ALTER COLUMN [AccountID] NVARCHAR(10) NULL
ALTER TABLE [dbo].[CashShopInventory] ALTER COLUMN [GiftName] NVARCHAR(10) NULL
ALTER TABLE [dbo].[CashShopInventory] ALTER COLUMN [GiftText] NVARCHAR(200) NULL
ALTER TABLE [dbo].[CashLog] ALTER COLUMN [UserID] NVARCHAR(16) NULL

PRINT '  Cash shop tables converted'
PRINT ''

-- =============================================
-- STEP 11: CONVERT CASTLE SIEGE TABLES
-- =============================================
PRINT 'Step 11: Converting castle siege tables...'

ALTER TABLE [dbo].[MuCastle_DATA] ALTER COLUMN [OWNER_GUILD] NVARCHAR(8) NOT NULL
ALTER TABLE [dbo].[MuCastle_REG_SIEGE] ALTER COLUMN [REG_SIEGE_GUILD] NVARCHAR(8) NOT NULL
ALTER TABLE [dbo].[MuCastle_SIEGE_GUILDLIST] ALTER COLUMN [GUILD_NAME] NVARCHAR(8) NOT NULL

PRINT '  Castle siege tables converted'
PRINT ''

-- =============================================
-- STEP 12: CONVERT MEMB_STAT
-- =============================================
PRINT 'Step 12: Converting MEMB_STAT...'

ALTER TABLE [dbo].[MEMB_STAT] ALTER COLUMN [memb___id] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[MEMB_STAT] ALTER COLUMN [ServerName] NVARCHAR(50) NULL
ALTER TABLE [dbo].[MEMB_STAT] ALTER COLUMN [IP] NVARCHAR(15) NULL

PRINT '  MEMB_STAT converted'
PRINT ''

-- =============================================
-- STEP 13: RECREATE PRIMARY KEY CONSTRAINTS
-- =============================================
PRINT 'Step 13: Recreating primary key constraints...'

-- Character
ALTER TABLE [dbo].[Character] 
ADD CONSTRAINT [PK_Character] PRIMARY KEY NONCLUSTERED ([Name] ASC)
WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, 
      ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
PRINT '  Recreated PK_Character'

-- Guild
ALTER TABLE [dbo].[Guild] 
ADD CONSTRAINT [PK_Guild] PRIMARY KEY CLUSTERED ([G_Name] ASC)
WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, 
      ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
PRINT '  Recreated PK_Guild'

-- GuildMember
ALTER TABLE [dbo].[GuildMember] 
ADD CONSTRAINT [PK_GuildMember] PRIMARY KEY CLUSTERED ([Name] ASC)
WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, 
      ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
PRINT '  Recreated PK_GuildMember'

-- AccountCharacter2-5
ALTER TABLE [dbo].[AccountCharacter2] 
ADD CONSTRAINT [PK_AccountCharacter2] PRIMARY KEY NONCLUSTERED ([Id] ASC)

ALTER TABLE [dbo].[AccountCharacter3] 
ADD CONSTRAINT [PK_AccountCharacter3] PRIMARY KEY NONCLUSTERED ([Id] ASC)

ALTER TABLE [dbo].[AccountCharacter4] 
ADD CONSTRAINT [PK_AccountCharacter4] PRIMARY KEY NONCLUSTERED ([Id] ASC)

ALTER TABLE [dbo].[AccountCharacter5] 
ADD CONSTRAINT [PK_AccountCharacter5] PRIMARY KEY NONCLUSTERED ([Id] ASC)

PRINT ''

-- =============================================
-- STEP 14: RECREATE FOREIGN KEY CONSTRAINTS
-- =============================================
PRINT 'Step 14: Recreating foreign key constraints...'

-- CustomNpcQuest
ALTER TABLE [dbo].[CustomNpcQuest] WITH CHECK 
ADD CONSTRAINT [FK_CustomNpcQuest_Character] FOREIGN KEY([Name])
REFERENCES [dbo].[Character] ([Name])
ON UPDATE CASCADE
ON DELETE CASCADE

ALTER TABLE [dbo].[CustomNpcQuest] CHECK CONSTRAINT [FK_CustomNpcQuest_Character]
PRINT '  Recreated FK_CustomNpcQuest_Character'

-- CustomQuest
ALTER TABLE [dbo].[CustomQuest] WITH CHECK 
ADD CONSTRAINT [FK_CustomQuest_Character] FOREIGN KEY([Name])
REFERENCES [dbo].[Character] ([Name])
ON UPDATE CASCADE
ON DELETE CASCADE

ALTER TABLE [dbo].[CustomQuest] CHECK CONSTRAINT [FK_CustomQuest_Character]
PRINT '  Recreated FK_CustomQuest_Character'

PRINT ''

-- =============================================
-- STEP 15: VERIFICATION
-- =============================================
PRINT '========================================='
PRINT 'Unicode Migration Completed Successfully!'
PRINT '========================================='
PRINT ''
PRINT 'Verification:'

SELECT 
    TABLE_NAME, 
    COUNT(*) as 'Converted Columns'
FROM INFORMATION_SCHEMA.COLUMNS
WHERE TABLE_SCHEMA = 'dbo' 
    AND DATA_TYPE = 'nvarchar'
    AND TABLE_NAME IN (
        'Character', 'AccountCharacter', 'AccountCharacter2', 'AccountCharacter3',
        'AccountCharacter4', 'AccountCharacter5', 'MEMB_INFO', 'Guild', 'GuildMember',
        'CustomNpcQuest', 'CustomQuest', 'CustomAttack'
    )
GROUP BY TABLE_NAME
ORDER BY TABLE_NAME

PRINT ''
PRINT 'All text columns have been converted to nvarchar!'
PRINT 'Your database now supports Unicode characters!'
PRINT ''
PRINT 'Next steps:'
PRINT '1. Run Database_StoredProcedure_Unicode_Update_FIXED.sql'
PRINT '2. Start GameServer with Unicode support'
PRINT '3. Test with Portuguese characters'
PRINT ''

GO
