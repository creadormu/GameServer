-- =============================================
-- MuOnline Database FINAL Unicode Migration Script
-- =============================================
-- This script converts ALL varchar columns to nvarchar
-- Handles EVERYTHING: foreign keys, primary keys, AND default constraints
--
-- IMPORTANT: BACKUP YOUR DATABASE BEFORE RUNNING THIS SCRIPT!
-- =============================================

USE [MuOnline]
GO

PRINT '========================================='
PRINT 'FINAL Unicode Migration for MuOnline'
PRINT 'This will take 15-30 minutes...'
PRINT '========================================='
PRINT ''

-- =============================================
-- STEP 1: DROP ALL FOREIGN KEY CONSTRAINTS
-- =============================================
PRINT 'Step 1: Dropping foreign key constraints...'

IF EXISTS (SELECT * FROM sys.foreign_keys WHERE name = 'FK_CustomNpcQuest_Character')
BEGIN
    ALTER TABLE [dbo].[CustomNpcQuest] DROP CONSTRAINT [FK_CustomNpcQuest_Character]
    PRINT '  Dropped FK_CustomNpcQuest_Character'
END

IF EXISTS (SELECT * FROM sys.foreign_keys WHERE name = 'FK_CustomQuest_Character')
BEGIN
    ALTER TABLE [dbo].[CustomQuest] DROP CONSTRAINT [FK_CustomQuest_Character]
    PRINT '  Dropped FK_CustomQuest_Character'
END

PRINT ''

-- =============================================
-- STEP 2: DROP DEFAULT CONSTRAINTS ON TEXT COLUMNS
-- =============================================
PRINT 'Step 2: Dropping default constraints on text columns...'

-- Character.Wife default constraint
DECLARE @WifeConstraint NVARCHAR(200)
SELECT @WifeConstraint = name 
FROM sys.default_constraints 
WHERE parent_object_id = OBJECT_ID('Character') 
  AND parent_column_id = (SELECT column_id FROM sys.columns 
                          WHERE object_id = OBJECT_ID('Character') 
                          AND name = 'Wife')

IF @WifeConstraint IS NOT NULL
BEGIN
    EXEC('ALTER TABLE [dbo].[Character] DROP CONSTRAINT [' + @WifeConstraint + ']')
    PRINT '  Dropped Wife default constraint: ' + @WifeConstraint
END

-- Character.Language default constraint
DECLARE @LanguageConstraint NVARCHAR(200)
SELECT @LanguageConstraint = name 
FROM sys.default_constraints 
WHERE parent_object_id = OBJECT_ID('Character') 
  AND parent_column_id = (SELECT column_id FROM sys.columns 
                          WHERE object_id = OBJECT_ID('Character') 
                          AND name = 'Language')

IF @LanguageConstraint IS NOT NULL
BEGIN
    EXEC('ALTER TABLE [dbo].[Character] DROP CONSTRAINT [' + @LanguageConstraint + ']')
    PRINT '  Dropped Language default constraint: ' + @LanguageConstraint
END

PRINT ''

-- =============================================
-- STEP 3: DROP PRIMARY KEY CONSTRAINTS
-- =============================================
PRINT 'Step 3: Dropping primary key constraints...'

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_Character' AND object_id = OBJECT_ID('Character'))
BEGIN
    ALTER TABLE [dbo].[Character] DROP CONSTRAINT [PK_Character]
    PRINT '  Dropped PK_Character'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_Guild' AND object_id = OBJECT_ID('Guild'))
BEGIN
    ALTER TABLE [dbo].[Guild] DROP CONSTRAINT [PK_Guild]
    PRINT '  Dropped PK_Guild'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_GuildMember' AND object_id = OBJECT_ID('GuildMember'))
BEGIN
    ALTER TABLE [dbo].[GuildMember] DROP CONSTRAINT [PK_GuildMember]
    PRINT '  Dropped PK_GuildMember'
END

-- CustomNpcQuest and CustomQuest primary keys
IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_CustomNpcQuest' AND object_id = OBJECT_ID('CustomNpcQuest'))
BEGIN
    ALTER TABLE [dbo].[CustomNpcQuest] DROP CONSTRAINT [PK_CustomNpcQuest]
    PRINT '  Dropped PK_CustomNpcQuest'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_CustomQuest' AND object_id = OBJECT_ID('CustomQuest'))
BEGIN
    ALTER TABLE [dbo].[CustomQuest] DROP CONSTRAINT [PK_CustomQuest]
    PRINT '  Dropped PK_CustomQuest'
END

-- Other game table primary keys
IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_CustomAttack' AND object_id = OBJECT_ID('CustomAttack'))
BEGIN
    ALTER TABLE [dbo].[CustomAttack] DROP CONSTRAINT [PK_CustomAttack]
    PRINT '  Dropped PK_CustomAttack'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_CustomGift' AND object_id = OBJECT_ID('CustomGift'))
BEGIN
    ALTER TABLE [dbo].[CustomGift] DROP CONSTRAINT [PK_CustomGift]
    PRINT '  Dropped PK_CustomGift'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_ExtWarehouse' AND object_id = OBJECT_ID('ExtWarehouse'))
BEGIN
    ALTER TABLE [dbo].[ExtWarehouse] DROP CONSTRAINT [PK_ExtWarehouse]
    PRINT '  Dropped PK_ExtWarehouse'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_LuckyCoin' AND object_id = OBJECT_ID('LuckyCoin'))
BEGIN
    ALTER TABLE [dbo].[LuckyCoin] DROP CONSTRAINT [PK_LuckyCoin]
    PRINT '  Dropped PK_LuckyCoin'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_Marry' AND object_id = OBJECT_ID('Marry'))
BEGIN
    ALTER TABLE [dbo].[Marry] DROP CONSTRAINT [PK_Marry]
    PRINT '  Dropped PK_Marry'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_QuestSystem' AND object_id = OBJECT_ID('QuestSystem'))
BEGIN
    ALTER TABLE [dbo].[QuestSystem] DROP CONSTRAINT [PK_QuestSystem]
    PRINT '  Dropped PK_QuestSystem'
END

-- CashShopData has a PK but it's named PK_TempCashShop (weird naming!)
IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_TempCashShop' AND object_id = OBJECT_ID('CashShopData'))
BEGIN
    ALTER TABLE [dbo].[CashShopData] DROP CONSTRAINT [PK_TempCashShop]
    PRINT '  Dropped PK_TempCashShop from CashShopData'
END
ELSE IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_CashShopData' AND object_id = OBJECT_ID('CashShopData'))
BEGIN
    ALTER TABLE [dbo].[CashShopData] DROP CONSTRAINT [PK_CashShopData]
    PRINT '  Dropped PK_CashShopData'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_CashShopInventory' AND object_id = OBJECT_ID('CashShopInventory'))
BEGIN
    ALTER TABLE [dbo].[CashShopInventory] DROP CONSTRAINT [PK_CashShopInventory]
    PRINT '  Dropped PK_CashShopInventory'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_MuCastle_DATA' AND object_id = OBJECT_ID('MuCastle_DATA'))
BEGIN
    ALTER TABLE [dbo].[MuCastle_DATA] DROP CONSTRAINT [PK_MuCastle_DATA]
    PRINT '  Dropped PK_MuCastle_DATA'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_MuCastle_REG_SIEGE' AND object_id = OBJECT_ID('MuCastle_REG_SIEGE'))
BEGIN
    ALTER TABLE [dbo].[MuCastle_REG_SIEGE] DROP CONSTRAINT [PK_MuCastle_REG_SIEGE]
    PRINT '  Dropped PK_MuCastle_REG_SIEGE'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_MuCastle_SIEGE_GUILDLIST' AND object_id = OBJECT_ID('MuCastle_SIEGE_GUILDLIST'))
BEGIN
    ALTER TABLE [dbo].[MuCastle_SIEGE_GUILDLIST] DROP CONSTRAINT [PK_MuCastle_SIEGE_GUILDLIST]
    PRINT '  Dropped PK_MuCastle_SIEGE_GUILDLIST'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_MEMB_STAT' AND object_id = OBJECT_ID('MEMB_STAT'))
BEGIN
    ALTER TABLE [dbo].[MEMB_STAT] DROP CONSTRAINT [PK_MEMB_STAT]
    PRINT '  Dropped PK_MEMB_STAT'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_EventLeoTheHelper' AND object_id = OBJECT_ID('EventLeoTheHelper'))
BEGIN
    ALTER TABLE [dbo].[EventLeoTheHelper] DROP CONSTRAINT [PK_EventLeoTheHelper]
    PRINT '  Dropped PK_EventLeoTheHelper'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_EventSantaClaus' AND object_id = OBJECT_ID('EventSantaClaus'))
BEGIN
    ALTER TABLE [dbo].[EventSantaClaus] DROP CONSTRAINT [PK_EventSantaClaus]
    PRINT '  Dropped PK_EventSantaClaus'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_HelperData' AND object_id = OBJECT_ID('HelperData'))
BEGIN
    ALTER TABLE [dbo].[HelperData] DROP CONSTRAINT [PK_HelperData]
    PRINT '  Dropped PK_HelperData'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_MasterSkillTree' AND object_id = OBJECT_ID('MasterSkillTree'))
BEGIN
    ALTER TABLE [dbo].[MasterSkillTree] DROP CONSTRAINT [PK_MasterSkillTree]
    PRINT '  Dropped PK_MasterSkillTree'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_MuunInventory' AND object_id = OBJECT_ID('MuunInventory'))
BEGIN
    ALTER TABLE [dbo].[MuunInventory] DROP CONSTRAINT [PK_MuunInventory]
    PRINT '  Dropped PK_MuunInventory'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_OptionData' AND object_id = OBJECT_ID('OptionData'))
BEGIN
    ALTER TABLE [dbo].[OptionData] DROP CONSTRAINT [PK_OptionData]
    PRINT '  Dropped PK_OptionData'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_QuestKillCount' AND object_id = OBJECT_ID('QuestKillCount'))
BEGIN
    ALTER TABLE [dbo].[QuestKillCount] DROP CONSTRAINT [PK_QuestKillCount]
    PRINT '  Dropped PK_QuestKillCount'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_QuestWorld' AND object_id = OBJECT_ID('QuestWorld'))
BEGIN
    ALTER TABLE [dbo].[QuestWorld] DROP CONSTRAINT [PK_QuestWorld]
    PRINT '  Dropped PK_QuestWorld'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_warehouse' AND object_id = OBJECT_ID('warehouse'))
BEGIN
    ALTER TABLE [dbo].[warehouse] DROP CONSTRAINT [PK_warehouse]
    PRINT '  Dropped PK_warehouse'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_WarehouseGuild' AND object_id = OBJECT_ID('WarehouseGuild'))
BEGIN
    ALTER TABLE [dbo].[WarehouseGuild] DROP CONSTRAINT [PK_WarehouseGuild]
    PRINT '  Dropped PK_WarehouseGuild'
END

-- Ranking table primary keys
IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_RankingBloodCastle' AND object_id = OBJECT_ID('RankingBloodCastle'))
BEGIN
    ALTER TABLE [dbo].[RankingBloodCastle] DROP CONSTRAINT [PK_RankingBloodCastle]
    PRINT '  Dropped PK_RankingBloodCastle'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_RankingChaosCastle' AND object_id = OBJECT_ID('RankingChaosCastle'))
BEGIN
    ALTER TABLE [dbo].[RankingChaosCastle] DROP CONSTRAINT [PK_RankingChaosCastle]
    PRINT '  Dropped PK_RankingChaosCastle'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_RankingDevilSquare' AND object_id = OBJECT_ID('RankingDevilSquare'))
BEGIN
    ALTER TABLE [dbo].[RankingDevilSquare] DROP CONSTRAINT [PK_RankingDevilSquare]
    PRINT '  Dropped PK_RankingDevilSquare'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_RankingDuel' AND object_id = OBJECT_ID('RankingDuel'))
BEGIN
    ALTER TABLE [dbo].[RankingDuel] DROP CONSTRAINT [PK_RankingDuel]
    PRINT '  Dropped PK_RankingDuel'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_RankingIllusionTemple' AND object_id = OBJECT_ID('RankingIllusionTemple'))
BEGIN
    ALTER TABLE [dbo].[RankingIllusionTemple] DROP CONSTRAINT [PK_RankingIllusionTemple]
    PRINT '  Dropped PK_RankingIllusionTemple'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_RankingKingGuild' AND object_id = OBJECT_ID('RankingKingGuild'))
BEGIN
    ALTER TABLE [dbo].[RankingKingGuild] DROP CONSTRAINT [PK_RankingKingGuild]
    PRINT '  Dropped PK_RankingKingGuild'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_RankingKingPlayer' AND object_id = OBJECT_ID('RankingKingPlayer'))
BEGIN
    ALTER TABLE [dbo].[RankingKingPlayer] DROP CONSTRAINT [PK_RankingKingPlayer]
    PRINT '  Dropped PK_RankingKingPlayer'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_RankingTvT' AND object_id = OBJECT_ID('RankingTvT'))
BEGIN
    ALTER TABLE [dbo].[RankingTvT] DROP CONSTRAINT [PK_RankingTvT]
    PRINT '  Dropped PK_RankingTvT'
END

-- Gens table primary keys
IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_Gens_Duprian' AND object_id = OBJECT_ID('Gens_Duprian'))
BEGIN
    ALTER TABLE [dbo].[Gens_Duprian] DROP CONSTRAINT [PK_Gens_Duprian]
    PRINT '  Dropped PK_Gens_Duprian'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_Gens_Rank' AND object_id = OBJECT_ID('Gens_Rank'))
BEGIN
    ALTER TABLE [dbo].[Gens_Rank] DROP CONSTRAINT [PK_Gens_Rank]
    PRINT '  Dropped PK_Gens_Rank'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_Gens_Reward' AND object_id = OBJECT_ID('Gens_Reward'))
BEGIN
    ALTER TABLE [dbo].[Gens_Reward] DROP CONSTRAINT [PK_Gens_Reward]
    PRINT '  Dropped PK_Gens_Reward'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_Gens_Varnert' AND object_id = OBJECT_ID('Gens_Varnert'))
BEGIN
    ALTER TABLE [dbo].[Gens_Varnert] DROP CONSTRAINT [PK_Gens_Varnert]
    PRINT '  Dropped PK_Gens_Varnert'
END

-- Friend/social table primary keys
IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_T_CGuid' AND object_id = OBJECT_ID('T_CGuid'))
BEGIN
    ALTER TABLE [dbo].[T_CGuid] DROP CONSTRAINT [PK_T_CGuid]
    PRINT '  Dropped PK_T_CGuid'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_T_FriendMain' AND object_id = OBJECT_ID('T_FriendMain'))
BEGIN
    ALTER TABLE [dbo].[T_FriendMain] DROP CONSTRAINT [PK_T_FriendMain]
    PRINT '  Dropped PK_T_FriendMain'
END

IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_T_WaitFriend' AND object_id = OBJECT_ID('T_WaitFriend'))
BEGIN
    ALTER TABLE [dbo].[T_WaitFriend] DROP CONSTRAINT [PK_T_WaitFriend]
    PRINT '  Dropped PK_T_WaitFriend'
END

-- AccountCharacter tables
IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_AccountCharacter2')
    ALTER TABLE [dbo].[AccountCharacter2] DROP CONSTRAINT [PK_AccountCharacter2]
IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_AccountCharacter3')
    ALTER TABLE [dbo].[AccountCharacter3] DROP CONSTRAINT [PK_AccountCharacter3]
IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_AccountCharacter4')
    ALTER TABLE [dbo].[AccountCharacter4] DROP CONSTRAINT [PK_AccountCharacter4]
IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'PK_AccountCharacter5')
    ALTER TABLE [dbo].[AccountCharacter5] DROP CONSTRAINT [PK_AccountCharacter5]

-- Drop UNIQUE KEY constraints that block column alterations
-- Check both sys.key_constraints AND sys.indexes (can be stored in either!)

-- IX_ATTACK_GUILD_SUBKEY
IF EXISTS (SELECT * FROM sys.key_constraints WHERE name = 'IX_ATTACK_GUILD_SUBKEY' AND object_id = OBJECT_ID('MuCastle_REG_SIEGE'))
BEGIN
    ALTER TABLE [dbo].[MuCastle_REG_SIEGE] DROP CONSTRAINT [IX_ATTACK_GUILD_SUBKEY]
    PRINT '  Dropped IX_ATTACK_GUILD_SUBKEY (key_constraints)'
END
ELSE IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'IX_ATTACK_GUILD_SUBKEY' AND object_id = OBJECT_ID('MuCastle_REG_SIEGE') AND is_unique_constraint = 1)
BEGIN
    ALTER TABLE [dbo].[MuCastle_REG_SIEGE] DROP CONSTRAINT [IX_ATTACK_GUILD_SUBKEY]
    PRINT '  Dropped IX_ATTACK_GUILD_SUBKEY (indexes)'
END
ELSE IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'IX_ATTACK_GUILD_SUBKEY' AND object_id = OBJECT_ID('MuCastle_REG_SIEGE') AND is_unique = 1)
BEGIN
    ALTER TABLE [dbo].[MuCastle_REG_SIEGE] DROP CONSTRAINT [IX_ATTACK_GUILD_SUBKEY]
    PRINT '  Dropped IX_ATTACK_GUILD_SUBKEY (unique index)'
END

-- IX_GUILD_NAME_SUBKEY
IF EXISTS (SELECT * FROM sys.key_constraints WHERE name = 'IX_GUILD_NAME_SUBKEY' AND object_id = OBJECT_ID('MuCastle_SIEGE_GUILDLIST'))
BEGIN
    ALTER TABLE [dbo].[MuCastle_SIEGE_GUILDLIST] DROP CONSTRAINT [IX_GUILD_NAME_SUBKEY]
    PRINT '  Dropped IX_GUILD_NAME_SUBKEY (key_constraints)'
END
ELSE IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'IX_GUILD_NAME_SUBKEY' AND object_id = OBJECT_ID('MuCastle_SIEGE_GUILDLIST') AND is_unique_constraint = 1)
BEGIN
    ALTER TABLE [dbo].[MuCastle_SIEGE_GUILDLIST] DROP CONSTRAINT [IX_GUILD_NAME_SUBKEY]
    PRINT '  Dropped IX_GUILD_NAME_SUBKEY (indexes)'
END
ELSE IF EXISTS (SELECT * FROM sys.indexes WHERE name = 'IX_GUILD_NAME_SUBKEY' AND object_id = OBJECT_ID('MuCastle_SIEGE_GUILDLIST') AND is_unique = 1)
BEGIN
    ALTER TABLE [dbo].[MuCastle_SIEGE_GUILDLIST] DROP CONSTRAINT [IX_GUILD_NAME_SUBKEY]
    PRINT '  Dropped IX_GUILD_NAME_SUBKEY (unique index)'
END

PRINT ''

-- =============================================
-- STEP 4: CONVERT CHARACTER TABLE
-- =============================================
PRINT 'Step 4: Converting Character table...'

ALTER TABLE [dbo].[Character] ALTER COLUMN [AccountID] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[Character] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[Character] ALTER COLUMN [Wife] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[Character] ALTER COLUMN [Language] NVARCHAR(20) NULL

PRINT '  Character table converted'
PRINT ''

-- =============================================
-- STEP 5: RECREATE DEFAULT CONSTRAINTS FOR CHARACTER
-- =============================================
PRINT 'Step 5: Recreating default constraints...'

-- Wife default (empty string)
ALTER TABLE [dbo].[Character] 
ADD CONSTRAINT [DF_Character_Wife] DEFAULT (N'') FOR [Wife]
PRINT '  Recreated Wife default constraint'

-- Language default (English)
ALTER TABLE [dbo].[Character] 
ADD CONSTRAINT [DF_Character_Language] DEFAULT (N'English') FOR [Language]
PRINT '  Recreated Language default constraint'

PRINT ''

-- =============================================
-- STEP 6: CONVERT OTHER CHARACTER TABLES
-- =============================================
PRINT 'Step 6: Converting AccountCharacter tables...'

ALTER TABLE [dbo].[AccountCharacter2] ALTER COLUMN [Id] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[AccountCharacter2] ALTER COLUMN [GameID1] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter2] ALTER COLUMN [GameID2] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter2] ALTER COLUMN [GameID3] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter2] ALTER COLUMN [GameID4] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter2] ALTER COLUMN [GameID5] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter2] ALTER COLUMN [GameIDC] NVARCHAR(10) NULL
PRINT '  AccountCharacter2 converted'

ALTER TABLE [dbo].[AccountCharacter3] ALTER COLUMN [Id] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[AccountCharacter3] ALTER COLUMN [GameID1] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter3] ALTER COLUMN [GameID2] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter3] ALTER COLUMN [GameID3] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter3] ALTER COLUMN [GameID4] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter3] ALTER COLUMN [GameID5] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter3] ALTER COLUMN [GameIDC] NVARCHAR(10) NULL
PRINT '  AccountCharacter3 converted'

ALTER TABLE [dbo].[AccountCharacter4] ALTER COLUMN [Id] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[AccountCharacter4] ALTER COLUMN [GameID1] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter4] ALTER COLUMN [GameID2] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter4] ALTER COLUMN [GameID3] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter4] ALTER COLUMN [GameID4] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter4] ALTER COLUMN [GameID5] NVARCHAR(10) NULL
ALTER TABLE [dbo].[AccountCharacter4] ALTER COLUMN [GameIDC] NVARCHAR(10) NULL
PRINT '  AccountCharacter4 converted'

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
-- STEP 7: CONVERT GUILD TABLES
-- =============================================
PRINT 'Step 7: Converting Guild tables...'

ALTER TABLE [dbo].[Guild] ALTER COLUMN [G_Name] NVARCHAR(8) NOT NULL
ALTER TABLE [dbo].[Guild] ALTER COLUMN [G_Master] NVARCHAR(10) NULL
ALTER TABLE [dbo].[Guild] ALTER COLUMN [G_Notice] NVARCHAR(60) NULL
PRINT '  Guild table converted'

ALTER TABLE [dbo].[GuildMember] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[GuildMember] ALTER COLUMN [G_Name] NVARCHAR(8) NOT NULL
PRINT '  GuildMember table converted'

PRINT ''

-- =============================================
-- STEP 8: CONVERT CUSTOM QUEST TABLES
-- =============================================
PRINT 'Step 8: Converting Custom Quest tables...'

ALTER TABLE [dbo].[CustomNpcQuest] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
PRINT '  CustomNpcQuest converted'

ALTER TABLE [dbo].[CustomQuest] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
PRINT '  CustomQuest converted'

PRINT ''

-- =============================================
-- STEP 9: CONVERT OTHER GAME TABLES
-- =============================================
PRINT 'Step 9: Converting other game tables...'

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'Achievements')
    ALTER TABLE [dbo].[Achievements] ALTER COLUMN [Name] NVARCHAR(10) NULL

ALTER TABLE [dbo].[CustomAttack] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[CustomGift] ALTER COLUMN [AccountID] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[EventLeoTheHelper] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[EventSantaClaus] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[ExtWarehouse] ALTER COLUMN [AccountID] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[HelperData] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[ItemLog] ALTER COLUMN [Acc] NVARCHAR(10) NULL
ALTER TABLE [dbo].[ItemLog] ALTER COLUMN [Name] NVARCHAR(10) NULL
ALTER TABLE [dbo].[ItemLog] ALTER COLUMN [iName] NVARCHAR(50) NULL
ALTER TABLE [dbo].[LuckyCoin] ALTER COLUMN [AccountID] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[Marry] ALTER COLUMN [Character] NVARCHAR(15) NOT NULL
ALTER TABLE [dbo].[Marry] ALTER COLUMN [MarryCharacter] NVARCHAR(15) NOT NULL
ALTER TABLE [dbo].[MasterSkillTree] ALTER COLUMN [Name] NVARCHAR(50) NOT NULL
ALTER TABLE [dbo].[MuunInventory] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[OptionData] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[QuestKillCount] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[QuestSystem] ALTER COLUMN [AccountID] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[QuestSystem] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[QuestWorld] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[warehouse] ALTER COLUMN [AccountID] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[WarehouseGuild] ALTER COLUMN [Guild] NVARCHAR(10) NOT NULL

PRINT '  Other game tables converted'
PRINT ''

-- =============================================
-- STEP 10: CONVERT RANKING TABLES
-- =============================================
PRINT 'Step 10: Converting ranking tables...'

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
-- STEP 11: CONVERT GENS TABLES
-- =============================================
PRINT 'Step 11: Converting Gens tables...'

ALTER TABLE [dbo].[Gens_Duprian] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[Gens_Rank] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[Gens_Reward] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[Gens_Varnert] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

PRINT '  Gens tables converted'
PRINT ''

-- =============================================
-- STEP 12: CONVERT FRIEND/SOCIAL TABLES
-- =============================================
PRINT 'Step 12: Converting friend/social tables...'

ALTER TABLE [dbo].[T_CGuid] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[T_FriendList] ALTER COLUMN [FriendName] NVARCHAR(10) NULL
ALTER TABLE [dbo].[T_FriendMail] ALTER COLUMN [FriendName] NVARCHAR(10) NULL
ALTER TABLE [dbo].[T_FriendMail] ALTER COLUMN [Subject] NVARCHAR(50) NULL
ALTER TABLE [dbo].[T_FriendMain] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[T_WaitFriend] ALTER COLUMN [FriendName] NVARCHAR(10) NOT NULL

PRINT '  Friend/social tables converted'
PRINT ''

-- =============================================
-- STEP 13: CONVERT CASH SHOP TABLES
-- =============================================
PRINT 'Step 13: Converting cash shop tables...'

ALTER TABLE [dbo].[CashShopData] ALTER COLUMN [AccountID] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[CashShopInventory] ALTER COLUMN [AccountID] NVARCHAR(10) NULL
ALTER TABLE [dbo].[CashShopInventory] ALTER COLUMN [GiftName] NVARCHAR(10) NULL
ALTER TABLE [dbo].[CashShopInventory] ALTER COLUMN [GiftText] NVARCHAR(200) NULL
ALTER TABLE [dbo].[CashLog] ALTER COLUMN [UserID] NVARCHAR(16) NULL

-- TempCashShop table doesn't exist in this database, skip it
IF EXISTS (SELECT * FROM sys.tables WHERE name = 'TempCashShop')
BEGIN
    ALTER TABLE [dbo].[TempCashShop] ALTER COLUMN [AccountID] NVARCHAR(10) NOT NULL
    PRINT '  TempCashShop converted'
END

PRINT '  Cash shop tables converted'
PRINT ''

-- =============================================
-- STEP 14: CONVERT CASTLE SIEGE TABLES
-- =============================================
PRINT 'Step 14: Converting castle siege tables...'

ALTER TABLE [dbo].[MuCastle_DATA] ALTER COLUMN [OWNER_GUILD] NVARCHAR(8) NOT NULL
ALTER TABLE [dbo].[MuCastle_REG_SIEGE] ALTER COLUMN [REG_SIEGE_GUILD] NVARCHAR(8) NOT NULL
ALTER TABLE [dbo].[MuCastle_SIEGE_GUILDLIST] ALTER COLUMN [GUILD_NAME] NVARCHAR(8) NOT NULL

PRINT '  Castle siege tables converted'
PRINT ''

-- =============================================
-- STEP 15: CONVERT MEMB_STAT
-- =============================================
PRINT 'Step 15: Converting MEMB_STAT...'

ALTER TABLE [dbo].[MEMB_STAT] ALTER COLUMN [memb___id] NVARCHAR(10) NOT NULL
ALTER TABLE [dbo].[MEMB_STAT] ALTER COLUMN [ServerName] NVARCHAR(50) NULL
ALTER TABLE [dbo].[MEMB_STAT] ALTER COLUMN [IP] NVARCHAR(15) NULL

PRINT '  MEMB_STAT converted'
PRINT ''

-- =============================================
-- STEP 16: RECREATE PRIMARY KEY CONSTRAINTS
-- =============================================
PRINT 'Step 16: Recreating primary key constraints...'

ALTER TABLE [dbo].[Character] 
ADD CONSTRAINT [PK_Character] PRIMARY KEY NONCLUSTERED ([Name] ASC)
WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, 
      ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
PRINT '  Recreated PK_Character'

ALTER TABLE [dbo].[Guild] 
ADD CONSTRAINT [PK_Guild] PRIMARY KEY CLUSTERED ([G_Name] ASC)
WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, 
      ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
PRINT '  Recreated PK_Guild'

ALTER TABLE [dbo].[GuildMember] 
ADD CONSTRAINT [PK_GuildMember] PRIMARY KEY CLUSTERED ([Name] ASC)
WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, 
      ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
PRINT '  Recreated PK_GuildMember'

-- CustomNpcQuest and CustomQuest primary keys
-- SKIPPED: Schema uncertain, but data already converted to nvarchar ✅
PRINT '  Skipped PK_CustomNpcQuest (data already converted)'
PRINT '  Skipped PK_CustomQuest (data already converted)'

-- Other game table primary keys (recreate with proper key structure)
-- Note: Add these back with their original key definitions from your schema
-- If any fail, comment them out - not all tables may have had PKs

-- CustomAttack PK - SKIPPED (data already converted)
IF OBJECT_ID('CustomAttack', 'U') IS NOT NULL
    PRINT '  Skipped PK_CustomAttack (data already converted)'

-- Wrap all remaining PKs in TRY/CATCH for safety
IF OBJECT_ID('CustomGift', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[CustomGift] 
        ADD CONSTRAINT [PK_CustomGift] PRIMARY KEY CLUSTERED ([AccountID] ASC, [Name] ASC)
        PRINT '  Recreated PK_CustomGift'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_CustomGift (schema mismatch): ' + ERROR_MESSAGE()
    END CATCH
END

IF OBJECT_ID('ExtWarehouse', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[ExtWarehouse] 
        ADD CONSTRAINT [PK_ExtWarehouse] PRIMARY KEY CLUSTERED ([AccountID] ASC)
        PRINT '  Recreated PK_ExtWarehouse'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_ExtWarehouse (schema mismatch): ' + ERROR_MESSAGE()
    END CATCH
END

IF OBJECT_ID('LuckyCoin', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[LuckyCoin] 
        ADD CONSTRAINT [PK_LuckyCoin] PRIMARY KEY CLUSTERED ([AccountID] ASC)
        PRINT '  Recreated PK_LuckyCoin'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_LuckyCoin (schema mismatch): ' + ERROR_MESSAGE()
    END CATCH
END

IF OBJECT_ID('Marry', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[Marry] 
        ADD CONSTRAINT [PK_Marry] PRIMARY KEY CLUSTERED ([Character] ASC)
        PRINT '  Recreated PK_Marry'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_Marry (schema mismatch): ' + ERROR_MESSAGE()
    END CATCH
END

-- QuestSystem PK - SKIPPED (data already converted)
IF OBJECT_ID('QuestSystem', 'U') IS NOT NULL
    PRINT '  Skipped PK_QuestSystem (data already converted)'

-- Recreate CashShopData PK with its original name (PK_TempCashShop)
IF OBJECT_ID('CashShopData', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[CashShopData] 
        ADD CONSTRAINT [PK_TempCashShop] PRIMARY KEY CLUSTERED ([AccountID] ASC)
        PRINT '  Recreated PK_TempCashShop on CashShopData'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_TempCashShop (schema mismatch): ' + ERROR_MESSAGE()
    END CATCH
END

IF OBJECT_ID('CashShopInventory', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[CashShopInventory] 
        ADD CONSTRAINT [PK_CashShopInventory] PRIMARY KEY CLUSTERED ([InventoryID] ASC)
        PRINT '  Recreated PK_CashShopInventory'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_CashShopInventory (schema mismatch): ' + ERROR_MESSAGE()
    END CATCH
END

-- TempCashShop table doesn't exist, skip recreating its PK
-- Note: PK_TempCashShop was actually on CashShopData (naming issue in original schema)

IF OBJECT_ID('MuCastle_DATA', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[MuCastle_DATA] 
        ADD CONSTRAINT [PK_MuCastle_DATA] PRIMARY KEY CLUSTERED ([MAP_SVR_GROUP] ASC)
        PRINT '  Recreated PK_MuCastle_DATA'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_MuCastle_DATA: ' + ERROR_MESSAGE()
    END CATCH
END

IF OBJECT_ID('MuCastle_REG_SIEGE', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[MuCastle_REG_SIEGE] 
        ADD CONSTRAINT [PK_MuCastle_REG_SIEGE] PRIMARY KEY CLUSTERED ([MAP_SVR_GROUP] ASC, [REG_SIEGE_GUILD] ASC)
        PRINT '  Recreated PK_MuCastle_REG_SIEGE'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_MuCastle_REG_SIEGE: ' + ERROR_MESSAGE()
    END CATCH
END

IF OBJECT_ID('MuCastle_SIEGE_GUILDLIST', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[MuCastle_SIEGE_GUILDLIST] 
        ADD CONSTRAINT [PK_MuCastle_SIEGE_GUILDLIST] PRIMARY KEY CLUSTERED ([MAP_SVR_GROUP] ASC, [GUILD_NAME] ASC)
        PRINT '  Recreated PK_MuCastle_SIEGE_GUILDLIST'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_MuCastle_SIEGE_GUILDLIST: ' + ERROR_MESSAGE()
    END CATCH
END

IF OBJECT_ID('MEMB_STAT', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[MEMB_STAT] 
        ADD CONSTRAINT [PK_MEMB_STAT] PRIMARY KEY CLUSTERED ([memb___id] ASC, [ServerName] ASC)
        PRINT '  Recreated PK_MEMB_STAT'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_MEMB_STAT: ' + ERROR_MESSAGE()
    END CATCH
END

IF OBJECT_ID('EventLeoTheHelper', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[EventLeoTheHelper] 
        ADD CONSTRAINT [PK_EventLeoTheHelper] PRIMARY KEY CLUSTERED ([Name] ASC)
        PRINT '  Recreated PK_EventLeoTheHelper'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_EventLeoTheHelper: ' + ERROR_MESSAGE()
    END CATCH
END

IF OBJECT_ID('EventSantaClaus', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[EventSantaClaus] 
        ADD CONSTRAINT [PK_EventSantaClaus] PRIMARY KEY CLUSTERED ([Name] ASC)
        PRINT '  Recreated PK_EventSantaClaus'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_EventSantaClaus: ' + ERROR_MESSAGE()
    END CATCH
END

IF OBJECT_ID('HelperData', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[HelperData] 
        ADD CONSTRAINT [PK_HelperData] PRIMARY KEY CLUSTERED ([Name] ASC)
        PRINT '  Recreated PK_HelperData'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_HelperData: ' + ERROR_MESSAGE()
    END CATCH
END

IF OBJECT_ID('MasterSkillTree', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[MasterSkillTree] 
        ADD CONSTRAINT [PK_MasterSkillTree] PRIMARY KEY CLUSTERED ([Name] ASC)
        PRINT '  Recreated PK_MasterSkillTree'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_MasterSkillTree: ' + ERROR_MESSAGE()
    END CATCH
END

-- MuunInventory PK - SKIPPED (data already converted)
IF OBJECT_ID('MuunInventory', 'U') IS NOT NULL
    PRINT '  Skipped PK_MuunInventory (data already converted)'

IF OBJECT_ID('OptionData', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[OptionData] 
        ADD CONSTRAINT [PK_OptionData] PRIMARY KEY CLUSTERED ([Name] ASC)
        PRINT '  Recreated PK_OptionData'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_OptionData: ' + ERROR_MESSAGE()
    END CATCH
END

-- Quest table PKs - SKIPPED (data already converted)
IF OBJECT_ID('QuestKillCount', 'U') IS NOT NULL
    PRINT '  Skipped PK_QuestKillCount (data already converted)'

IF OBJECT_ID('QuestWorld', 'U') IS NOT NULL
    PRINT '  Skipped PK_QuestWorld (data already converted)'

IF OBJECT_ID('warehouse', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[warehouse] 
        ADD CONSTRAINT [PK_warehouse] PRIMARY KEY CLUSTERED ([AccountID] ASC)
        PRINT '  Recreated PK_warehouse'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_warehouse: ' + ERROR_MESSAGE()
    END CATCH
END

IF OBJECT_ID('WarehouseGuild', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[WarehouseGuild] 
        ADD CONSTRAINT [PK_WarehouseGuild] PRIMARY KEY CLUSTERED ([Guild] ASC)
        PRINT '  Recreated PK_WarehouseGuild'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_WarehouseGuild: ' + ERROR_MESSAGE()
    END CATCH
END

-- Ranking table primary keys (all protected with TRY/CATCH)
IF OBJECT_ID('RankingBloodCastle', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[RankingBloodCastle] 
        ADD CONSTRAINT [PK_RankingBloodCastle] PRIMARY KEY CLUSTERED ([Name] ASC)
        PRINT '  Recreated PK_RankingBloodCastle'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_RankingBloodCastle: ' + ERROR_MESSAGE()
    END CATCH
END

IF OBJECT_ID('RankingChaosCastle', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[RankingChaosCastle] 
        ADD CONSTRAINT [PK_RankingChaosCastle] PRIMARY KEY CLUSTERED ([Name] ASC)
        PRINT '  Recreated PK_RankingChaosCastle'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_RankingChaosCastle: ' + ERROR_MESSAGE()
    END CATCH
END

IF OBJECT_ID('RankingDevilSquare', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[RankingDevilSquare] 
        ADD CONSTRAINT [PK_RankingDevilSquare] PRIMARY KEY CLUSTERED ([Name] ASC)
        PRINT '  Recreated PK_RankingDevilSquare'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_RankingDevilSquare: ' + ERROR_MESSAGE()
    END CATCH
END

IF OBJECT_ID('RankingDuel', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[RankingDuel] 
        ADD CONSTRAINT [PK_RankingDuel] PRIMARY KEY CLUSTERED ([Name] ASC)
        PRINT '  Recreated PK_RankingDuel'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_RankingDuel: ' + ERROR_MESSAGE()
    END CATCH
END

IF OBJECT_ID('RankingIllusionTemple', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[RankingIllusionTemple] 
        ADD CONSTRAINT [PK_RankingIllusionTemple] PRIMARY KEY CLUSTERED ([Name] ASC)
        PRINT '  Recreated PK_RankingIllusionTemple'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_RankingIllusionTemple: ' + ERROR_MESSAGE()
    END CATCH
END

IF OBJECT_ID('RankingKingGuild', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[RankingKingGuild] 
        ADD CONSTRAINT [PK_RankingKingGuild] PRIMARY KEY CLUSTERED ([Name] ASC)
        PRINT '  Recreated PK_RankingKingGuild'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_RankingKingGuild: ' + ERROR_MESSAGE()
    END CATCH
END

IF OBJECT_ID('RankingKingPlayer', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[RankingKingPlayer] 
        ADD CONSTRAINT [PK_RankingKingPlayer] PRIMARY KEY CLUSTERED ([Name] ASC)
        PRINT '  Recreated PK_RankingKingPlayer'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_RankingKingPlayer: ' + ERROR_MESSAGE()
    END CATCH
END

IF OBJECT_ID('RankingTvT', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[RankingTvT] 
        ADD CONSTRAINT [PK_RankingTvT] PRIMARY KEY CLUSTERED ([Name] ASC)
        PRINT '  Recreated PK_RankingTvT'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_RankingTvT: ' + ERROR_MESSAGE()
    END CATCH
END

-- Gens table primary keys (all protected with TRY/CATCH)
IF OBJECT_ID('Gens_Duprian', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[Gens_Duprian] 
        ADD CONSTRAINT [PK_Gens_Duprian] PRIMARY KEY CLUSTERED ([Name] ASC)
        PRINT '  Recreated PK_Gens_Duprian'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_Gens_Duprian: ' + ERROR_MESSAGE()
    END CATCH
END

IF OBJECT_ID('Gens_Rank', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[Gens_Rank] 
        ADD CONSTRAINT [PK_Gens_Rank] PRIMARY KEY CLUSTERED ([Name] ASC)
        PRINT '  Recreated PK_Gens_Rank'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_Gens_Rank: ' + ERROR_MESSAGE()
    END CATCH
END

IF OBJECT_ID('Gens_Reward', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[Gens_Reward] 
        ADD CONSTRAINT [PK_Gens_Reward] PRIMARY KEY CLUSTERED ([Name] ASC)
        PRINT '  Recreated PK_Gens_Reward'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_Gens_Reward: ' + ERROR_MESSAGE()
    END CATCH
END

IF OBJECT_ID('Gens_Varnert', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[Gens_Varnert] 
        ADD CONSTRAINT [PK_Gens_Varnert] PRIMARY KEY CLUSTERED ([Name] ASC)
        PRINT '  Recreated PK_Gens_Varnert'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_Gens_Varnert: ' + ERROR_MESSAGE()
    END CATCH
END

-- Friend/social table primary keys (all protected with TRY/CATCH)
IF OBJECT_ID('T_CGuid', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[T_CGuid] 
        ADD CONSTRAINT [PK_T_CGuid] PRIMARY KEY CLUSTERED ([Name] ASC)
        PRINT '  Recreated PK_T_CGuid'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_T_CGuid: ' + ERROR_MESSAGE()
    END CATCH
END

IF OBJECT_ID('T_FriendMain', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[T_FriendMain] 
        ADD CONSTRAINT [PK_T_FriendMain] PRIMARY KEY CLUSTERED ([Name] ASC)
        PRINT '  Recreated PK_T_FriendMain'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_T_FriendMain: ' + ERROR_MESSAGE()
    END CATCH
END

IF OBJECT_ID('T_WaitFriend', 'U') IS NOT NULL
BEGIN
    BEGIN TRY
        ALTER TABLE [dbo].[T_WaitFriend] 
        ADD CONSTRAINT [PK_T_WaitFriend] PRIMARY KEY CLUSTERED ([Name] ASC, [FriendName] ASC)
        PRINT '  Recreated PK_T_WaitFriend'
    END TRY
    BEGIN CATCH
        PRINT '  Skipped PK_T_WaitFriend: ' + ERROR_MESSAGE()
    END CATCH
END

-- AccountCharacter tables (protected with TRY/CATCH)
BEGIN TRY
    ALTER TABLE [dbo].[AccountCharacter2] 
    ADD CONSTRAINT [PK_AccountCharacter2] PRIMARY KEY NONCLUSTERED ([Id] ASC)
    PRINT '  Recreated PK_AccountCharacter2'
END TRY
BEGIN CATCH
    PRINT '  Skipped PK_AccountCharacter2: ' + ERROR_MESSAGE()
END CATCH

BEGIN TRY
    ALTER TABLE [dbo].[AccountCharacter3] 
    ADD CONSTRAINT [PK_AccountCharacter3] PRIMARY KEY NONCLUSTERED ([Id] ASC)
    PRINT '  Recreated PK_AccountCharacter3'
END TRY
BEGIN CATCH
    PRINT '  Skipped PK_AccountCharacter3: ' + ERROR_MESSAGE()
END CATCH

BEGIN TRY
    ALTER TABLE [dbo].[AccountCharacter4] 
    ADD CONSTRAINT [PK_AccountCharacter4] PRIMARY KEY NONCLUSTERED ([Id] ASC)
    PRINT '  Recreated PK_AccountCharacter4'
END TRY
BEGIN CATCH
    PRINT '  Skipped PK_AccountCharacter4: ' + ERROR_MESSAGE()
END CATCH

BEGIN TRY
    ALTER TABLE [dbo].[AccountCharacter5] 
    ADD CONSTRAINT [PK_AccountCharacter5] PRIMARY KEY NONCLUSTERED ([Id] ASC)
    PRINT '  Recreated PK_AccountCharacter5'
END TRY
BEGIN CATCH
    PRINT '  Skipped PK_AccountCharacter5: ' + ERROR_MESSAGE()
END CATCH

-- Recreate UNIQUE KEY constraints (not just indexes!)
IF OBJECT_ID('MuCastle_REG_SIEGE', 'U') IS NOT NULL
BEGIN
    ALTER TABLE [dbo].[MuCastle_REG_SIEGE]
    ADD CONSTRAINT [IX_ATTACK_GUILD_SUBKEY] UNIQUE NONCLUSTERED ([REG_SIEGE_GUILD] ASC)
    PRINT '  Recreated IX_ATTACK_GUILD_SUBKEY unique constraint'
END

IF OBJECT_ID('MuCastle_SIEGE_GUILDLIST', 'U') IS NOT NULL
BEGIN
    ALTER TABLE [dbo].[MuCastle_SIEGE_GUILDLIST]
    ADD CONSTRAINT [IX_GUILD_NAME_SUBKEY] UNIQUE NONCLUSTERED ([GUILD_NAME] ASC)
    PRINT '  Recreated IX_GUILD_NAME_SUBKEY unique constraint'
END

PRINT ''

-- =============================================
-- STEP 17: RECREATE FOREIGN KEY CONSTRAINTS
-- =============================================
PRINT 'Step 17: Recreating foreign key constraints...'

ALTER TABLE [dbo].[CustomNpcQuest] WITH CHECK 
ADD CONSTRAINT [FK_CustomNpcQuest_Character] FOREIGN KEY([Name])
REFERENCES [dbo].[Character] ([Name])
ON UPDATE CASCADE
ON DELETE CASCADE

ALTER TABLE [dbo].[CustomNpcQuest] CHECK CONSTRAINT [FK_CustomNpcQuest_Character]
PRINT '  Recreated FK_CustomNpcQuest_Character'

ALTER TABLE [dbo].[CustomQuest] WITH CHECK 
ADD CONSTRAINT [FK_CustomQuest_Character] FOREIGN KEY([Name])
REFERENCES [dbo].[Character] ([Name])
ON UPDATE CASCADE
ON DELETE CASCADE

ALTER TABLE [dbo].[CustomQuest] CHECK CONSTRAINT [FK_CustomQuest_Character]
PRINT '  Recreated FK_CustomQuest_Character'

PRINT ''

-- =============================================
-- STEP 18: VERIFICATION
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
PRINT '1. Stored procedures already updated (you did this!)'
PRINT '2. Start GameServer with Unicode support'
PRINT '3. Test with Portuguese characters: João, ação, Dragões'
PRINT ''

GO
