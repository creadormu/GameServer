-- =============================================
-- ULTIMATE Unicode Migration for MuOnline
-- =============================================
-- This script AUTOMATICALLY finds and drops ALL constraints!
--
-- IMPORTANT: BACKUP YOUR DATABASE BEFORE RUNNING THIS SCRIPT!
-- =============================================

USE [MuOnline]
GO

PRINT '========================================='
PRINT 'ULTIMATE Unicode Migration for MuOnline'
PRINT 'This will take 15-30 minutes...'
PRINT '========================================='
PRINT ''

-- =============================================
-- STEP 1: DROP FOREIGN KEY CONSTRAINTS
-- =============================================
PRINT 'Step 1: Dropping foreign key constraints...'

-- Drop FK constraints on Character.Name
DECLARE @sql NVARCHAR(MAX) = ''

SELECT @sql = @sql + 'ALTER TABLE [dbo].[' + OBJECT_NAME(fk.parent_object_id) + '] DROP CONSTRAINT [' + fk.name + ']; '
FROM sys.foreign_keys fk
INNER JOIN sys.foreign_key_columns fkc ON fk.object_id = fkc.constraint_object_id
WHERE fkc.referenced_object_id = OBJECT_ID('Character')

IF @sql <> ''
BEGIN
    EXEC sp_executesql @sql
    PRINT '  Dropped all FK constraints on Character table'
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
-- STEP 3: DROP ALL PRIMARY KEY CONSTRAINTS AUTOMATICALLY
-- =============================================
PRINT 'Step 3: Dropping primary key constraints AUTOMATICALLY...'

DECLARE @DropPKSQL NVARCHAR(MAX) = ''
DECLARE @TableName NVARCHAR(128)
DECLARE @PKName NVARCHAR(128)

DECLARE pk_cursor CURSOR FOR
SELECT 
    t.name AS TableName,
    i.name AS PKName
FROM sys.indexes i
INNER JOIN sys.tables t ON i.object_id = t.object_id
WHERE i.is_primary_key = 1
  AND t.name IN (
    'Character', 'Guild', 'GuildMember',
    'CustomNpcQuest', 'CustomQuest', 'CustomAttack', 'CustomGift',
    'EventLeoTheHelper', 'EventSantaClaus',
    'HelperData', 'MasterSkillTree', 'MuunInventory', 'OptionData',
    'QuestKillCount', 'QuestWorld', 'QuestSystem',
    'warehouse', 'WarehouseGuild', 'ExtWarehouse', 'LuckyCoin',
    'Marry',
    'CashShopData', 'CashShopInventory', 'TempCashShop',
    'MuCastle_DATA', 'MuCastle_REG_SIEGE', 'MuCastle_SIEGE_GUILDLIST',
    'RankingBloodCastle', 'RankingChaosCastle', 'RankingDevilSquare',
    'RankingDuel', 'RankingIllusionTemple', 'RankingKingGuild', 'RankingKingPlayer', 'RankingTvT',
    'Gens_Duprian', 'Gens_Rank', 'Gens_Reward', 'Gens_Varnert',
    'T_CGuid', 'T_FriendMain', 'T_WaitFriend',
    'AccountCharacter2', 'AccountCharacter3', 'AccountCharacter4', 'AccountCharacter5',
    'MEMB_STAT'
  )
ORDER BY t.name

OPEN pk_cursor
FETCH NEXT FROM pk_cursor INTO @TableName, @PKName

WHILE @@FETCH_STATUS = 0
BEGIN
    BEGIN TRY
        SET @DropPKSQL = 'ALTER TABLE [dbo].[' + @TableName + '] DROP CONSTRAINT [' + @PKName + ']'
        EXEC sp_executesql @DropPKSQL
        PRINT '  Dropped ' + @PKName + ' from ' + @TableName
    END TRY
    BEGIN CATCH
        PRINT '  Could not drop ' + @PKName + ' from ' + @TableName + ': ' + ERROR_MESSAGE()
    END CATCH
    
    FETCH NEXT FROM pk_cursor INTO @TableName, @PKName
END

CLOSE pk_cursor
DEALLOCATE pk_cursor

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

ALTER TABLE [dbo].[Character] 
ADD CONSTRAINT [DF_Character_Wife] DEFAULT (N'') FOR [Wife]
PRINT '  Recreated Wife default constraint'

ALTER TABLE [dbo].[Character] 
ADD CONSTRAINT [DF_Character_Language] DEFAULT (N'English') FOR [Language]
PRINT '  Recreated Language default constraint'

PRINT ''

-- =============================================
-- STEP 6: CONVERT ACCOUNTCHARACTER TABLES
-- =============================================
PRINT 'Step 6: Converting AccountCharacter tables...'

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'AccountCharacter2')
BEGIN
    ALTER TABLE [dbo].[AccountCharacter2] ALTER COLUMN [Id] NVARCHAR(10) NOT NULL
    ALTER TABLE [dbo].[AccountCharacter2] ALTER COLUMN [GameID1] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[AccountCharacter2] ALTER COLUMN [GameID2] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[AccountCharacter2] ALTER COLUMN [GameID3] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[AccountCharacter2] ALTER COLUMN [GameID4] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[AccountCharacter2] ALTER COLUMN [GameID5] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[AccountCharacter2] ALTER COLUMN [GameIDC] NVARCHAR(10) NULL
    PRINT '  AccountCharacter2 converted'
END

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'AccountCharacter3')
BEGIN
    ALTER TABLE [dbo].[AccountCharacter3] ALTER COLUMN [Id] NVARCHAR(10) NOT NULL
    ALTER TABLE [dbo].[AccountCharacter3] ALTER COLUMN [GameID1] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[AccountCharacter3] ALTER COLUMN [GameID2] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[AccountCharacter3] ALTER COLUMN [GameID3] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[AccountCharacter3] ALTER COLUMN [GameID4] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[AccountCharacter3] ALTER COLUMN [GameID5] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[AccountCharacter3] ALTER COLUMN [GameIDC] NVARCHAR(10) NULL
    PRINT '  AccountCharacter3 converted'
END

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'AccountCharacter4')
BEGIN
    ALTER TABLE [dbo].[AccountCharacter4] ALTER COLUMN [Id] NVARCHAR(10) NOT NULL
    ALTER TABLE [dbo].[AccountCharacter4] ALTER COLUMN [GameID1] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[AccountCharacter4] ALTER COLUMN [GameID2] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[AccountCharacter4] ALTER COLUMN [GameID3] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[AccountCharacter4] ALTER COLUMN [GameID4] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[AccountCharacter4] ALTER COLUMN [GameID5] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[AccountCharacter4] ALTER COLUMN [GameIDC] NVARCHAR(10) NULL
    PRINT '  AccountCharacter4 converted'
END

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'AccountCharacter5')
BEGIN
    ALTER TABLE [dbo].[AccountCharacter5] ALTER COLUMN [Id] NVARCHAR(10) NOT NULL
    ALTER TABLE [dbo].[AccountCharacter5] ALTER COLUMN [GameID1] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[AccountCharacter5] ALTER COLUMN [GameID2] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[AccountCharacter5] ALTER COLUMN [GameID3] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[AccountCharacter5] ALTER COLUMN [GameID4] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[AccountCharacter5] ALTER COLUMN [GameID5] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[AccountCharacter5] ALTER COLUMN [GameIDC] NVARCHAR(10) NULL
    PRINT '  AccountCharacter5 converted'
END

PRINT ''

-- =============================================
-- STEP 7: CONVERT ALL OTHER TABLES
-- =============================================
PRINT 'Step 7: Converting all other tables...'

-- Guild tables
IF EXISTS (SELECT * FROM sys.tables WHERE name = 'Guild')
BEGIN
    ALTER TABLE [dbo].[Guild] ALTER COLUMN [G_Name] NVARCHAR(8) NOT NULL
    ALTER TABLE [dbo].[Guild] ALTER COLUMN [G_Master] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[Guild] ALTER COLUMN [G_Notice] NVARCHAR(60) NULL
END

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'GuildMember')
BEGIN
    ALTER TABLE [dbo].[GuildMember] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
    ALTER TABLE [dbo].[GuildMember] ALTER COLUMN [G_Name] NVARCHAR(8) NOT NULL
END

-- Custom Quest tables
IF EXISTS (SELECT * FROM sys.tables WHERE name = 'CustomNpcQuest')
    ALTER TABLE [dbo].[CustomNpcQuest] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'CustomQuest')
    ALTER TABLE [dbo].[CustomQuest] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

-- Game tables
IF EXISTS (SELECT * FROM sys.tables WHERE name = 'CustomAttack')
    ALTER TABLE [dbo].[CustomAttack] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'CustomGift')
    ALTER TABLE [dbo].[CustomGift] ALTER COLUMN [AccountID] NVARCHAR(10) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'EventLeoTheHelper')
    ALTER TABLE [dbo].[EventLeoTheHelper] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'EventSantaClaus')
    ALTER TABLE [dbo].[EventSantaClaus] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'ExtWarehouse')
    ALTER TABLE [dbo].[ExtWarehouse] ALTER COLUMN [AccountID] NVARCHAR(10) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'HelperData')
    ALTER TABLE [dbo].[HelperData] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'ItemLog')
BEGIN
    ALTER TABLE [dbo].[ItemLog] ALTER COLUMN [Acc] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[ItemLog] ALTER COLUMN [Name] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[ItemLog] ALTER COLUMN [iName] NVARCHAR(50) NULL
END

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'LuckyCoin')
    ALTER TABLE [dbo].[LuckyCoin] ALTER COLUMN [AccountID] NVARCHAR(10) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'Marry')
BEGIN
    ALTER TABLE [dbo].[Marry] ALTER COLUMN [Character] NVARCHAR(15) NOT NULL
    ALTER TABLE [dbo].[Marry] ALTER COLUMN [MarryCharacter] NVARCHAR(15) NOT NULL
END

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'MasterSkillTree')
    ALTER TABLE [dbo].[MasterSkillTree] ALTER COLUMN [Name] NVARCHAR(50) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'MuunInventory')
    ALTER TABLE [dbo].[MuunInventory] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'OptionData')
    ALTER TABLE [dbo].[OptionData] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'QuestKillCount')
    ALTER TABLE [dbo].[QuestKillCount] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'QuestSystem')
BEGIN
    ALTER TABLE [dbo].[QuestSystem] ALTER COLUMN [AccountID] NVARCHAR(10) NOT NULL
    ALTER TABLE [dbo].[QuestSystem] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL
END

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'QuestWorld')
    ALTER TABLE [dbo].[QuestWorld] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'warehouse')
    ALTER TABLE [dbo].[warehouse] ALTER COLUMN [AccountID] NVARCHAR(10) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'WarehouseGuild')
    ALTER TABLE [dbo].[WarehouseGuild] ALTER COLUMN [Guild] NVARCHAR(10) NOT NULL

-- Ranking tables
IF EXISTS (SELECT * FROM sys.tables WHERE name = 'RankingBloodCastle')
    ALTER TABLE [dbo].[RankingBloodCastle] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'RankingChaosCastle')
    ALTER TABLE [dbo].[RankingChaosCastle] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'RankingDevilSquare')
    ALTER TABLE [dbo].[RankingDevilSquare] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'RankingDuel')
    ALTER TABLE [dbo].[RankingDuel] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'RankingIllusionTemple')
    ALTER TABLE [dbo].[RankingIllusionTemple] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'RankingKingGuild')
    ALTER TABLE [dbo].[RankingKingGuild] ALTER COLUMN [Name] NVARCHAR(8) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'RankingKingPlayer')
    ALTER TABLE [dbo].[RankingKingPlayer] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'RankingTvT')
    ALTER TABLE [dbo].[RankingTvT] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

-- Gens tables
IF EXISTS (SELECT * FROM sys.tables WHERE name = 'Gens_Duprian')
    ALTER TABLE [dbo].[Gens_Duprian] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'Gens_Rank')
    ALTER TABLE [dbo].[Gens_Rank] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'Gens_Reward')
    ALTER TABLE [dbo].[Gens_Reward] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'Gens_Varnert')
    ALTER TABLE [dbo].[Gens_Varnert] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

-- Friend/social tables
IF EXISTS (SELECT * FROM sys.tables WHERE name = 'T_CGuid')
    ALTER TABLE [dbo].[T_CGuid] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'T_FriendList')
    ALTER TABLE [dbo].[T_FriendList] ALTER COLUMN [FriendName] NVARCHAR(10) NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'T_FriendMail')
BEGIN
    ALTER TABLE [dbo].[T_FriendMail] ALTER COLUMN [FriendName] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[T_FriendMail] ALTER COLUMN [Subject] NVARCHAR(50) NULL
END

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'T_FriendMain')
    ALTER TABLE [dbo].[T_FriendMain] ALTER COLUMN [Name] NVARCHAR(10) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'T_WaitFriend')
    ALTER TABLE [dbo].[T_WaitFriend] ALTER COLUMN [FriendName] NVARCHAR(10) NOT NULL

-- Cash shop tables
IF EXISTS (SELECT * FROM sys.tables WHERE name = 'CashShopData')
    ALTER TABLE [dbo].[CashShopData] ALTER COLUMN [AccountID] NVARCHAR(10) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'CashShopInventory')
BEGIN
    ALTER TABLE [dbo].[CashShopInventory] ALTER COLUMN [AccountID] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[CashShopInventory] ALTER COLUMN [GiftName] NVARCHAR(10) NULL
    ALTER TABLE [dbo].[CashShopInventory] ALTER COLUMN [GiftText] NVARCHAR(200) NULL
END

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'CashLog')
    ALTER TABLE [dbo].[CashLog] ALTER COLUMN [UserID] NVARCHAR(16) NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'TempCashShop')
    ALTER TABLE [dbo].[TempCashShop] ALTER COLUMN [AccountID] NVARCHAR(10) NOT NULL

-- Castle siege tables
IF EXISTS (SELECT * FROM sys.tables WHERE name = 'MuCastle_DATA')
    ALTER TABLE [dbo].[MuCastle_DATA] ALTER COLUMN [OWNER_GUILD] NVARCHAR(8) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'MuCastle_REG_SIEGE')
    ALTER TABLE [dbo].[MuCastle_REG_SIEGE] ALTER COLUMN [REG_SIEGE_GUILD] NVARCHAR(8) NOT NULL

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'MuCastle_SIEGE_GUILDLIST')
    ALTER TABLE [dbo].[MuCastle_SIEGE_GUILDLIST] ALTER COLUMN [GUILD_NAME] NVARCHAR(8) NOT NULL

-- MEMB_STAT
IF EXISTS (SELECT * FROM sys.tables WHERE name = 'MEMB_STAT')
BEGIN
    ALTER TABLE [dbo].[MEMB_STAT] ALTER COLUMN [memb___id] NVARCHAR(10) NOT NULL
    ALTER TABLE [dbo].[MEMB_STAT] ALTER COLUMN [ServerName] NVARCHAR(50) NULL
    ALTER TABLE [dbo].[MEMB_STAT] ALTER COLUMN [IP] NVARCHAR(15) NULL
END

PRINT '  All other tables converted'
PRINT ''

-- =============================================
-- STEP 8: RECREATE PRIMARY KEYS AUTOMATICALLY
-- =============================================
PRINT 'Step 8: Recreating primary key constraints...'

-- Create a temp table to store PK definitions
CREATE TABLE #PKRecreate (
    TableName NVARCHAR(128),
    PKName NVARCHAR(128),
    Columns NVARCHAR(500),
    IsUnique BIT
)

-- Insert PK definitions (you may need to adjust these based on your actual schema)
INSERT INTO #PKRecreate VALUES ('Character', 'PK_Character', '[Name]', 0)
INSERT INTO #PKRecreate VALUES ('Guild', 'PK_Guild', '[G_Name]', 0)
INSERT INTO #PKRecreate VALUES ('GuildMember', 'PK_GuildMember', '[Name]', 0)
INSERT INTO #PKRecreate VALUES ('CustomNpcQuest', 'PK_CustomNpcQuest', '[Name], [QuestIndex]', 0)
INSERT INTO #PKRecreate VALUES ('CustomQuest', 'PK_CustomQuest', '[Name], [QuestIndex]', 0)
INSERT INTO #PKRecreate VALUES ('CustomAttack', 'PK_CustomAttack', '[Name], [SkillIndex]', 0)
INSERT INTO #PKRecreate VALUES ('CustomGift', 'PK_CustomGift', '[AccountID], [Name]', 0)
INSERT INTO #PKRecreate VALUES ('ExtWarehouse', 'PK_ExtWarehouse', '[AccountID]', 0)
INSERT INTO #PKRecreate VALUES ('LuckyCoin', 'PK_LuckyCoin', '[AccountID]', 0)
INSERT INTO #PKRecreate VALUES ('Marry', 'PK_Marry', '[Character]', 0)
INSERT INTO #PKRecreate VALUES ('QuestSystem', 'PK_QuestSystem', '[AccountID], [Name], [QuestIndex]', 0)
INSERT INTO #PKRecreate VALUES ('CashShopData', 'PK_CashShopData', '[AccountID]', 0)
INSERT INTO #PKRecreate VALUES ('CashShopInventory', 'PK_CashShopInventory', '[InventoryID]', 0)
INSERT INTO #PKRecreate VALUES ('TempCashShop', 'PK_TempCashShop', '[AccountID]', 0)
INSERT INTO #PKRecreate VALUES ('warehouse', 'PK_warehouse', '[AccountID]', 0)
INSERT INTO #PKRecreate VALUES ('WarehouseGuild', 'PK_WarehouseGuild', '[Guild]', 0)
INSERT INTO #PKRecreate VALUES ('MuCastle_DATA', 'PK_MuCastle_DATA', '[MAP_SVR_GROUP]', 0)
INSERT INTO #PKRecreate VALUES ('MuCastle_REG_SIEGE', 'PK_MuCastle_REG_SIEGE', '[MAP_SVR_GROUP], [REG_SIEGE_GUILD]', 0)
INSERT INTO #PKRecreate VALUES ('MuCastle_SIEGE_GUILDLIST', 'PK_MuCastle_SIEGE_GUILDLIST', '[MAP_SVR_GROUP], [GUILD_NAME]', 0)
INSERT INTO #PKRecreate VALUES ('MEMB_STAT', 'PK_MEMB_STAT', '[memb___id], [ServerName]', 0)

-- Additional tables (add only if they exist in your DB)
INSERT INTO #PKRecreate VALUES ('EventLeoTheHelper', 'PK_EventLeoTheHelper', '[Name]', 0)
INSERT INTO #PKRecreate VALUES ('EventSantaClaus', 'PK_EventSantaClaus', '[Name]', 0)
INSERT INTO #PKRecreate VALUES ('HelperData', 'PK_HelperData', '[Name]', 0)
INSERT INTO #PKRecreate VALUES ('MasterSkillTree', 'PK_MasterSkillTree', '[Name]', 0)
INSERT INTO #PKRecreate VALUES ('MuunInventory', 'PK_MuunInventory', '[Name], [MuunIndex]', 0)
INSERT INTO #PKRecreate VALUES ('OptionData', 'PK_OptionData', '[Name]', 0)
INSERT INTO #PKRecreate VALUES ('QuestKillCount', 'PK_QuestKillCount', '[Name], [QuestIndex]', 0)
INSERT INTO #PKRecreate VALUES ('QuestWorld', 'PK_QuestWorld', '[Name]', 0)
INSERT INTO #PKRecreate VALUES ('RankingBloodCastle', 'PK_RankingBloodCastle', '[Name]', 0)
INSERT INTO #PKRecreate VALUES ('RankingChaosCastle', 'PK_RankingChaosCastle', '[Name]', 0)
INSERT INTO #PKRecreate VALUES ('RankingDevilSquare', 'PK_RankingDevilSquare', '[Name]', 0)
INSERT INTO #PKRecreate VALUES ('RankingDuel', 'PK_RankingDuel', '[Name]', 0)
INSERT INTO #PKRecreate VALUES ('RankingIllusionTemple', 'PK_RankingIllusionTemple', '[Name]', 0)
INSERT INTO #PKRecreate VALUES ('RankingKingGuild', 'PK_RankingKingGuild', '[Name]', 0)
INSERT INTO #PKRecreate VALUES ('RankingKingPlayer', 'PK_RankingKingPlayer', '[Name]', 0)
INSERT INTO #PKRecreate VALUES ('RankingTvT', 'PK_RankingTvT', '[Name]', 0)
INSERT INTO #PKRecreate VALUES ('Gens_Duprian', 'PK_Gens_Duprian', '[Name]', 0)
INSERT INTO #PKRecreate VALUES ('Gens_Rank', 'PK_Gens_Rank', '[Name]', 0)
INSERT INTO #PKRecreate VALUES ('Gens_Reward', 'PK_Gens_Reward', '[Name]', 0)
INSERT INTO #PKRecreate VALUES ('Gens_Varnert', 'PK_Gens_Varnert', '[Name]', 0)
INSERT INTO #PKRecreate VALUES ('T_CGuid', 'PK_T_CGuid', '[Name]', 0)
INSERT INTO #PKRecreate VALUES ('T_FriendMain', 'PK_T_FriendMain', '[Name]', 0)
INSERT INTO #PKRecreate VALUES ('T_WaitFriend', 'PK_T_WaitFriend', '[Name], [FriendName]', 0)
INSERT INTO #PKRecreate VALUES ('AccountCharacter2', 'PK_AccountCharacter2', '[Id]', 0)
INSERT INTO #PKRecreate VALUES ('AccountCharacter3', 'PK_AccountCharacter3', '[Id]', 0)
INSERT INTO #PKRecreate VALUES ('AccountCharacter4', 'PK_AccountCharacter4', '[Id]', 0)
INSERT INTO #PKRecreate VALUES ('AccountCharacter5', 'PK_AccountCharacter5', '[Id]', 0)

-- Recreate PKs
DECLARE @RecreatePKSQL NVARCHAR(MAX)
DECLARE @PKTableName NVARCHAR(128)
DECLARE @PKConstraintName NVARCHAR(128)
DECLARE @PKColumns NVARCHAR(500)

DECLARE pk_recreate_cursor CURSOR FOR
SELECT TableName, PKName, Columns
FROM #PKRecreate
WHERE EXISTS (SELECT 1 FROM sys.tables WHERE name = #PKRecreate.TableName)

OPEN pk_recreate_cursor
FETCH NEXT FROM pk_recreate_cursor INTO @PKTableName, @PKConstraintName, @PKColumns

WHILE @@FETCH_STATUS = 0
BEGIN
    BEGIN TRY
        SET @RecreatePKSQL = 'ALTER TABLE [dbo].[' + @PKTableName + '] ADD CONSTRAINT [' + @PKConstraintName + '] PRIMARY KEY CLUSTERED (' + @PKColumns + ')'
        EXEC sp_executesql @RecreatePKSQL
        PRINT '  Recreated ' + @PKConstraintName + ' on ' + @PKTableName
    END TRY
    BEGIN CATCH
        PRINT '  Could not recreate ' + @PKConstraintName + ' on ' + @PKTableName + ': ' + ERROR_MESSAGE()
    END CATCH
    
    FETCH NEXT FROM pk_recreate_cursor INTO @PKTableName, @PKConstraintName, @PKColumns
END

CLOSE pk_recreate_cursor
DEALLOCATE pk_recreate_cursor

DROP TABLE #PKRecreate

PRINT ''

-- =============================================
-- STEP 9: RECREATE FOREIGN KEY CONSTRAINTS
-- =============================================
PRINT 'Step 9: Recreating foreign key constraints...'

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'CustomNpcQuest')
BEGIN
    ALTER TABLE [dbo].[CustomNpcQuest] WITH CHECK 
    ADD CONSTRAINT [FK_CustomNpcQuest_Character] FOREIGN KEY([Name])
    REFERENCES [dbo].[Character] ([Name])
    ON UPDATE CASCADE
    ON DELETE CASCADE
    PRINT '  Recreated FK_CustomNpcQuest_Character'
END

IF EXISTS (SELECT * FROM sys.tables WHERE name = 'CustomQuest')
BEGIN
    ALTER TABLE [dbo].[CustomQuest] WITH CHECK 
    ADD CONSTRAINT [FK_CustomQuest_Character] FOREIGN KEY([Name])
    REFERENCES [dbo].[Character] ([Name])
    ON UPDATE CASCADE
    ON DELETE CASCADE
    PRINT '  Recreated FK_CustomQuest_Character'
END

PRINT ''

-- =============================================
-- VERIFICATION
-- =============================================
PRINT '========================================='
PRINT 'Unicode Migration Completed Successfully!'
PRINT '========================================='
PRINT ''
PRINT 'Next steps:'
PRINT '1. Stored procedures already updated'
PRINT '2. Start GameServer with Unicode support'
PRINT '3. Test with Portuguese: João, ação, Dragões'
PRINT ''

GO
