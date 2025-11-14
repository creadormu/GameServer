-- =============================================
-- Find ALL Indexes (not just Primary Keys)
-- =============================================
-- Use this to discover ALL indexes that might block ALTER COLUMN

USE [MuOnline]
GO

PRINT '========================================='
PRINT 'ALL INDEXES IN DATABASE'
PRINT '========================================='
PRINT ''

SELECT 
    t.name AS TableName,
    i.name AS IndexName,
    i.type_desc AS IndexType,
    i.is_primary_key AS IsPrimaryKey,
    i.is_unique_constraint AS IsUniqueConstraint,
    COL_NAME(ic.object_id, ic.column_id) AS ColumnName
FROM sys.indexes i
INNER JOIN sys.tables t ON i.object_id = t.object_id
INNER JOIN sys.index_columns ic ON i.object_id = ic.object_id AND i.index_id = ic.index_id
WHERE t.name IN (
    'Character', 'Guild', 'GuildMember',
    'CustomNpcQuest', 'CustomQuest', 'CustomAttack', 'CustomGift',
    'MuCastle_REG_SIEGE', 'MuCastle_SIEGE_GUILDLIST', 'MuCastle_DATA',
    'CashShopData', 'CashShopInventory', 'TempCashShop',
    'warehouse', 'ExtWarehouse', 'LuckyCoin',
    'T_FriendMain', 'T_CGuid', 'T_WaitFriend',
    'Marry', 'QuestSystem'
)
AND i.name IS NOT NULL
ORDER BY t.name, i.is_primary_key DESC, i.name

PRINT ''
PRINT '========================================='
PRINT 'DROP statements for non-PK indexes:'
PRINT '========================================='
PRINT ''

-- Generate DROP INDEX statements for non-primary key indexes
SELECT 
    'DROP INDEX [' + i.name + '] ON [dbo].[' + t.name + '];' AS DropStatement
FROM sys.indexes i
INNER JOIN sys.tables t ON i.object_id = t.object_id
WHERE i.is_primary_key = 0
  AND i.name IS NOT NULL
  AND t.name IN (
    'Character', 'Guild', 'GuildMember',
    'CustomNpcQuest', 'CustomQuest', 'CustomAttack', 'CustomGift',
    'MuCastle_REG_SIEGE', 'MuCastle_SIEGE_GUILDLIST', 'MuCastle_DATA',
    'CashShopData', 'CashShopInventory', 'TempCashShop',
    'warehouse', 'ExtWarehouse', 'LuckyCoin'
)
ORDER BY t.name, i.name

GO
