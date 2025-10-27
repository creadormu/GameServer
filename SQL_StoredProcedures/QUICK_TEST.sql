USE [MuOnline]
GO

-- =============================================
-- QUICK TEST SCRIPT
-- =============================================
-- Run this after installation to verify everything works
-- =============================================

PRINT '=========================================='
PRINT 'Bot System Quick Test'
PRINT '=========================================='
PRINT ''

-- Test 1: Check Tables
PRINT 'TEST 1: Checking tables...'
IF EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[BotNames]'))
    PRINT '  ✓ BotNames table exists'
ELSE
    PRINT '  ✗ BotNames table MISSING!'

IF EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[BotClassConfigs]'))
    PRINT '  ✓ BotClassConfigs table exists'
ELSE
    PRINT '  ✗ BotClassConfigs table MISSING!'

DECLARE @nameCount INT = (SELECT COUNT(*) FROM BotNames)
DECLARE @configCount INT = (SELECT COUNT(*) FROM BotClassConfigs)

PRINT '  → BotNames records: ' + CAST(@nameCount AS VARCHAR)
PRINT '  → BotClassConfigs records: ' + CAST(@configCount AS VARCHAR)
PRINT ''

-- Test 2: Check Stored Procedures
PRINT 'TEST 2: Checking stored procedures...'
IF EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[WZ_CreateBot]'))
    PRINT '  ✓ WZ_CreateBot exists'
ELSE
    PRINT '  ✗ WZ_CreateBot MISSING!'

IF EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[WZ_CreateBotBatch]'))
    PRINT '  ✓ WZ_CreateBotBatch exists'
ELSE
    PRINT '  ✗ WZ_CreateBotBatch MISSING!'

IF EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[WZ_GetRandomBotName]'))
    PRINT '  ✓ WZ_GetRandomBotName exists'
ELSE
    PRINT '  ✗ WZ_GetRandomBotName MISSING!'
PRINT ''

-- Test 3: Name Generation
PRINT 'TEST 3: Testing name generation...'
DECLARE @testName VARCHAR(50)

EXEC WZ_GetRandomBotName 'Male', 'English', @testName OUTPUT
PRINT '  → Random English male name: ' + @testName

EXEC WZ_GetRandomBotName 'Female', 'Spanish', @testName OUTPUT
PRINT '  → Random Spanish female name: ' + @testName

EXEC WZ_GetRandomBotName 'Male', NULL, @testName OUTPUT
PRINT '  → Random any language male name: ' + @testName
PRINT ''

-- Test 4: Config Retrieval
PRINT 'TEST 4: Testing config retrieval...'
DECLARE @str INT, @dex INT, @vit INT, @ene INT, @cmd INT
DECLARE @skill1 INT, @skill2 INT, @buff1 INT, @buff2 INT, @buff3 INT
DECLARE @inv VARCHAR(MAX), @magic VARCHAR(MAX)

EXEC WZ_GetBotClassConfig 0, 0, @str OUTPUT, @dex OUTPUT, @vit OUTPUT, @ene OUTPUT, @cmd OUTPUT,
    @skill1 OUTPUT, @skill2 OUTPUT, @buff1 OUTPUT, @buff2 OUTPUT, @buff3 OUTPUT,
    @inv OUTPUT, @magic OUTPUT

IF @str IS NOT NULL
BEGIN
    PRINT '  ✓ DW Config 0 retrieved successfully'
    PRINT '    Stats: STR=' + CAST(@str AS VARCHAR) + ' DEX=' + CAST(@dex AS VARCHAR) + 
          ' VIT=' + CAST(@vit AS VARCHAR) + ' ENE=' + CAST(@ene AS VARCHAR)
    PRINT '    Skills: Main=' + CAST(@skill1 AS VARCHAR) + ' Secondary=' + CAST(@skill2 AS VARCHAR)
    
    IF LEN(@inv) > 100
        PRINT '    ✓ InventoryHex is configured'
    ELSE
        PRINT '    ✗ InventoryHex NEEDS UPDATE (using default placeholder)'
        
    IF LEN(@magic) > 100
        PRINT '    ✓ MagicListHex is configured'
    ELSE
        PRINT '    ✗ MagicListHex NEEDS UPDATE (using default placeholder)'
END
ELSE
    PRINT '  ✗ Failed to retrieve config'
PRINT ''

-- Test 5: Random Config Selection
PRINT 'TEST 5: Testing random config selection...'
DECLARE @randConfig INT
EXEC WZ_GetRandomConfigIndex 0, 127, @randConfig OUTPUT
PRINT '  → Random config for DW (all enabled): Config ' + CAST(@randConfig AS VARCHAR)

EXEC WZ_GetRandomConfigIndex 16, 7, @randConfig OUTPUT
PRINT '  → Random config for DK (configs 0,1,2 enabled): Config ' + CAST(@randConfig AS VARCHAR)
PRINT ''

-- Test 6: Create Test Bot
PRINT 'TEST 6: Creating test bot...'
PRINT '  → Attempting to create: TestBot9999 (Zeus99)'

-- Clean up if exists
IF EXISTS (SELECT 1 FROM Character WHERE Name = 'Zeus99')
BEGIN
    DELETE FROM Character WHERE Name = 'Zeus99'
    PRINT '  → Cleaned up existing test bot'
END

DECLARE @result TINYINT
EXEC WZ_CreateBot 'TestBot9999', 'Zeus99', 0, 0, 400, 0, 125, 125, 20000000

-- Check if created
IF EXISTS (SELECT 1 FROM Character WHERE Name = 'Zeus99')
BEGIN
    PRINT '  ✓ Test bot created successfully!'
    
    SELECT 
        '  → Account: ' + AccountID AS Info,
        '  → Name: ' + Name,
        '  → Class: ' + CAST(Class AS VARCHAR),
        '  → Level: ' + CAST(cLevel AS VARCHAR)
    FROM Character 
    WHERE Name = 'Zeus99'
    
    -- Clean up
    DELETE FROM Character WHERE Name = 'Zeus99'
    DELETE FROM AccountCharacter WHERE Id = 'TestBot9999'
    DELETE FROM MEMB_INFO WHERE memb___id = 'TestBot9999'
    PRINT '  → Test bot cleaned up'
END
ELSE
    PRINT '  ✗ Test bot creation FAILED!'
PRINT ''

-- Test 7: Batch Creation Test (Optional - comment out if you don't want to create actual bots)
PRINT 'TEST 7: Batch creation test (OPTIONAL - COMMENTED OUT BY DEFAULT)'
PRINT '  → Uncomment the code below to test batch creation of 5 bots'
PRINT ''

/*
PRINT '  → Creating 5 test bots (Bot9991-Bot9995)...'
EXEC WZ_CreateBotBatch 9991, 5, -1, -1, 350, 400, 0, 125, 125, 0, NULL

IF (SELECT COUNT(*) FROM Character WHERE AccountID LIKE 'Bot999%') >= 5
BEGIN
    PRINT '  ✓ Batch creation successful!'
    
    SELECT 
        AccountID, Name, Class, cLevel
    FROM Character 
    WHERE AccountID LIKE 'Bot999%'
    
    -- Clean up test bots
    DELETE FROM Character WHERE AccountID LIKE 'Bot999%'
    DELETE FROM AccountCharacter WHERE Id LIKE 'Bot999%'
    DELETE FROM MEMB_INFO WHERE memb___id LIKE 'Bot999%'
    PRINT '  → Test bots cleaned up'
END
ELSE
    PRINT '  ✗ Batch creation FAILED!'
*/

-- Summary
PRINT '=========================================='
PRINT 'Test Summary'
PRINT '=========================================='
PRINT ''
PRINT 'Installation Status:'

IF @nameCount > 900 AND @configCount >= 49
    PRINT '  ✓ Tables populated correctly'
ELSE
    PRINT '  ✗ Tables need attention'

IF EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[WZ_CreateBotBatch]'))
    PRINT '  ✓ Stored procedures installed'
ELSE
    PRINT '  ✗ Stored procedures missing'

PRINT ''
PRINT 'Next Steps:'
PRINT '1. Update InventoryHex and MagicListHex in BotClassConfigs table'
PRINT '   Use: 05_UpdateConfigsTemplate.sql'
PRINT ''
PRINT '2. Test with GameServer C++ code:'
PRINT '   CreateMultipleBotsAdvanced_StoredProc(10, 1, ...)'
PRINT ''
PRINT '3. For manual SQL testing:'
PRINT '   EXEC WZ_CreateBotBatch 1, 10, -1, -1, 350, 400, 0, 125, 125, 0, NULL'
PRINT ''
PRINT '=========================================='
GO
