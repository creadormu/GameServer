-- =============================================
-- AGGRESSIVE PK DROPPER - Find ALL Primary Keys
-- =============================================
-- Run this first to see ALL primary keys in your database

USE [MuOnline]
GO

PRINT '========================================='
PRINT 'ALL PRIMARY KEYS IN DATABASE'
PRINT '========================================='
PRINT ''

SELECT 
    t.name AS TableName,
    i.name AS ConstraintName,
    i.type_desc AS ConstraintType
FROM sys.indexes i
INNER JOIN sys.tables t ON i.object_id = t.object_id
WHERE i.is_primary_key = 1
ORDER BY t.name

PRINT ''
PRINT '========================================='
PRINT 'Drop statements for ALL found PKs:'
PRINT '========================================='
PRINT ''

-- Generate DROP statements for all primary keys
SELECT 
    'ALTER TABLE [dbo].[' + t.name + '] DROP CONSTRAINT [' + i.name + '];' AS DropStatement
FROM sys.indexes i
INNER JOIN sys.tables t ON i.object_id = t.object_id
WHERE i.is_primary_key = 1
ORDER BY t.name

GO
