USE [MuOnline]
GO

-- =============================================
-- Stored Procedure: Get Random Bot Name
-- =============================================
IF EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[WZ_GetRandomBotName]') AND type in (N'P', N'PC'))
    DROP PROCEDURE [dbo].[WZ_GetRandomBotName]
GO

CREATE PROCEDURE [dbo].[WZ_GetRandomBotName]
    @Gender VARCHAR(10) = 'Male',  -- 'Male' or 'Female'
    @Language VARCHAR(20) = NULL,   -- Specific language or NULL for random
    @RandomName VARCHAR(50) OUTPUT
AS
BEGIN
    SET NOCOUNT ON
    
    DECLARE @NameID INT
    
    -- If no language specified, pick from all languages
    IF @Language IS NULL OR @Language = ''
    BEGIN
        SELECT TOP 1 @NameID = NameID, @RandomName = Name
        FROM BotNames
        WHERE Gender = @Gender AND IsUsed = 0
        ORDER BY NEWID()
    END
    ELSE
    BEGIN
        SELECT TOP 1 @NameID = NameID, @RandomName = Name
        FROM BotNames
        WHERE Gender = @Gender AND Language = @Language AND IsUsed = 0
        ORDER BY NEWID()
    END
    
    -- Mark as used (optional - uncomment if you want unique names)
    -- UPDATE BotNames SET IsUsed = 1 WHERE NameID = @NameID
    
    -- If no name found, return default
    IF @RandomName IS NULL
    BEGIN
        SET @RandomName = 'BotUser'
    END
    
    RETURN 0
END
GO

-- =============================================
-- Stored Procedure: Get Class Configuration
-- =============================================
IF EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[WZ_GetBotClassConfig]') AND type in (N'P', N'PC'))
    DROP PROCEDURE [dbo].[WZ_GetBotClassConfig]
GO

CREATE PROCEDURE [dbo].[WZ_GetBotClassConfig]
    @ClassCode INT,
    @ConfigIndex INT,
    @Strength INT OUTPUT,
    @Dexterity INT OUTPUT,
    @Vitality INT OUTPUT,
    @Energy INT OUTPUT,
    @Leadership INT OUTPUT,
    @MainSkill INT OUTPUT,
    @SecondarySkill INT OUTPUT,
    @Buff1 INT OUTPUT,
    @Buff2 INT OUTPUT,
    @Buff3 INT OUTPUT,
    @InventoryHex VARCHAR(MAX) OUTPUT,
    @MagicListHex VARCHAR(MAX) OUTPUT
AS
BEGIN
    SET NOCOUNT ON
    
    SELECT 
        @Strength = Strength,
        @Dexterity = Dexterity,
        @Vitality = Vitality,
        @Energy = Energy,
        @Leadership = Leadership,
        @MainSkill = MainSkill,
        @SecondarySkill = SecondarySkill,
        @Buff1 = Buff1,
        @Buff2 = Buff2,
        @Buff3 = Buff3,
        @InventoryHex = InventoryHex,
        @MagicListHex = MagicListHex
    FROM BotClassConfigs
    WHERE ClassCode = @ClassCode AND ConfigIndex = @ConfigIndex AND IsEnabled = 1
    
    IF @@ROWCOUNT = 0
    BEGIN
        RETURN 1 -- Configuration not found
    END
    
    RETURN 0
END
GO

-- =============================================
-- Stored Procedure: Get Random Enabled Config Index
-- =============================================
IF EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[WZ_GetRandomConfigIndex]') AND type in (N'P', N'PC'))
    DROP PROCEDURE [dbo].[WZ_GetRandomConfigIndex]
GO

CREATE PROCEDURE [dbo].[WZ_GetRandomConfigIndex]
    @ClassCode INT,
    @EnabledConfigsMask INT = 127,  -- Default: all 7 configs enabled (binary 1111111)
    @ConfigIndex INT OUTPUT
AS
BEGIN
    SET NOCOUNT ON
    
    -- Get random enabled config for this class
    SELECT TOP 1 @ConfigIndex = ConfigIndex
    FROM BotClassConfigs
    WHERE ClassCode = @ClassCode 
        AND IsEnabled = 1
        AND ((@EnabledConfigsMask & POWER(2, ConfigIndex)) > 0)
    ORDER BY NEWID()
    
    IF @ConfigIndex IS NULL
    BEGIN
        SET @ConfigIndex = 0  -- Default to first config
    END
    
    RETURN 0
END
GO

-- =============================================
-- Stored Procedure: Create Bot Character
-- =============================================
IF EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[WZ_CreateBot]') AND type in (N'P', N'PC'))
    DROP PROCEDURE [dbo].[WZ_CreateBot]
GO

CREATE PROCEDURE [dbo].[WZ_CreateBot]
    @AccountID VARCHAR(10),
    @CharName VARCHAR(10),
    @ClassCode INT,
    @ConfigIndex INT,
    @Level INT = 400,
    @MapNumber INT = 0,
    @MapPosX INT = 125,
    @MapPosY INT = 125,
    @Money INT = 20000000
AS
BEGIN
    SET NOCOUNT ON
    SET XACT_ABORT ON
    
    DECLARE @Result TINYINT
    DECLARE @Strength INT, @Dexterity INT, @Vitality INT, @Energy INT, @Leadership INT
    DECLARE @MainSkill INT, @SecondarySkill INT
    DECLARE @Buff1 INT, @Buff2 INT, @Buff3 INT
    DECLARE @InventoryHex VARCHAR(MAX), @MagicListHex VARCHAR(MAX)
    
    SET @Result = 0x00
    
    -- Check if character already exists
    IF EXISTS (SELECT Name FROM Character WHERE Name = @CharName)
    BEGIN
        SET @Result = 0x01  -- Character name already exists
        GOTO ProcEnd
    END
    
    BEGIN TRAN
    
    -- Get class configuration
    EXEC WZ_GetBotClassConfig 
        @ClassCode, @ConfigIndex,
        @Strength OUTPUT, @Dexterity OUTPUT, @Vitality OUTPUT, 
        @Energy OUTPUT, @Leadership OUTPUT,
        @MainSkill OUTPUT, @SecondarySkill OUTPUT,
        @Buff1 OUTPUT, @Buff2 OUTPUT, @Buff3 OUTPUT,
        @InventoryHex OUTPUT, @MagicListHex OUTPUT
    
    IF @InventoryHex IS NULL
    BEGIN
        SET @Result = 0x04  -- Configuration not found
        GOTO TranProcEnd
    END
    
    -- Create or update MEMB_INFO account
    IF NOT EXISTS (SELECT memb___id FROM MEMB_INFO WHERE memb___id = @AccountID)
    BEGIN
        INSERT INTO MEMB_INFO (memb___id, memb__pwd, memb_name, sno__numb, mail_addr, bloc_code, ctl1_code)
        VALUES (@AccountID, '123456', @AccountID, '123456789', 'bot@email.com', 0, 0)
        
        IF @@ERROR <> 0
        BEGIN
            SET @Result = 0x02  -- Failed to create account
            GOTO TranProcEnd
        END
    END
    
    -- Manage AccountCharacter slots
    IF NOT EXISTS (SELECT Id FROM AccountCharacter WHERE Id = @AccountID)
    BEGIN
        INSERT INTO AccountCharacter(Id, GameID1, GameID2, GameID3, GameID4, GameID5, GameIDC)
        VALUES(@AccountID, @CharName, NULL, NULL, NULL, NULL, NULL)
        
        SET @Result = @@ERROR
    END
    ELSE
    BEGIN
        DECLARE @g1 VARCHAR(10), @g2 VARCHAR(10), @g3 VARCHAR(10), @g4 VARCHAR(10), @g5 VARCHAR(10)
        SELECT @g1 = GameID1, @g2 = GameID2, @g3 = GameID3, @g4 = GameID4, @g5 = GameID5 
        FROM AccountCharacter WHERE Id = @AccountID
        
        IF (@g1 IS NULL OR LEN(@g1) = 0)
            UPDATE AccountCharacter SET GameID1 = @CharName WHERE Id = @AccountID
        ELSE IF (@g2 IS NULL OR LEN(@g2) = 0)
            UPDATE AccountCharacter SET GameID2 = @CharName WHERE Id = @AccountID
        ELSE IF (@g3 IS NULL OR LEN(@g3) = 0)
            UPDATE AccountCharacter SET GameID3 = @CharName WHERE Id = @AccountID
        ELSE IF (@g4 IS NULL OR LEN(@g4) = 0)
            UPDATE AccountCharacter SET GameID4 = @CharName WHERE Id = @AccountID
        ELSE IF (@g5 IS NULL OR LEN(@g5) = 0)
            UPDATE AccountCharacter SET GameID5 = @CharName WHERE Id = @AccountID
        ELSE
        BEGIN
            SET @Result = 0x03  -- No empty slot
            GOTO TranProcEnd
        END
        
        SET @Result = @@ERROR
    END
    
    IF (@Result <> 0)
        GOTO TranProcEnd
    
    -- Create Character
    INSERT INTO Character (
        AccountID, Name, cLevel, Class, 
        Strength, Dexterity, Vitality, Energy, Leadership,
        Money, MapNumber, MapPosX, MapPosY,
        Inventory, MagicList,
        LevelUpPoint, Experience,
        MDate, LDate, Quest, DbVersion
    )
    VALUES (
        @AccountID, @CharName, @Level, @ClassCode,
        @Strength, @Dexterity, @Vitality, @Energy, @Leadership,
        @Money, @MapNumber, @MapPosX, @MapPosY,
        CONVERT(VARBINARY(MAX), @InventoryHex, 1),
        CONVERT(VARBINARY(MAX), @MagicListHex, 1),
        0, 0,
        GETDATE(), GETDATE(), 0x1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F, 0
    )
    
    SET @Result = @@ERROR
    
TranProcEnd:
    IF (@Result <> 0)
        ROLLBACK TRAN
    ELSE
        COMMIT TRAN

ProcEnd:
    SET NOCOUNT OFF
    SET XACT_ABORT OFF
    
    -- Return result
    -- 0x00 = Configuration error, 0x01 = Success, 0x02 = Failed, 0x03 = No slot, 0x04 = Config not found
    SELECT
        CASE @Result
            WHEN 0x00 THEN 0x01  -- Success
            WHEN 0x01 THEN 0x00  -- Character exists
            WHEN 0x03 THEN 0x03  -- No empty slot
            WHEN 0x04 THEN 0x04  -- Config not found
            ELSE 0x02            -- Other error
        END AS Result
END
GO

-- =============================================
-- Stored Procedure: Create Multiple Bots (Batch)
-- =============================================
IF EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[WZ_CreateBotBatch]') AND type in (N'P', N'PC'))
    DROP PROCEDURE [dbo].[WZ_CreateBotBatch]
GO

CREATE PROCEDURE [dbo].[WZ_CreateBotBatch]
    @StartBotNumber INT,
    @BotCount INT,
    @ClassCode INT,              -- -1 for random
    @ConfigIndex INT,            -- -1 for random
    @MinLevel INT = 350,
    @MaxLevel INT = 400,
    @MapNumber INT = 0,
    @MapX INT = 125,
    @MapY INT = 125,
    @UseFemaleName BIT = 0,      -- 0=Male, 1=Female
    @Language VARCHAR(20) = NULL  -- NULL for random language
AS
BEGIN
    SET NOCOUNT ON
    
    DECLARE @i INT = 0
    DECLARE @AccountID VARCHAR(10)
    DECLARE @CharName VARCHAR(50)
    DECLARE @FullCharName VARCHAR(10)
    DECLARE @CurrentClass INT
    DECLARE @CurrentConfig INT
    DECLARE @CurrentLevel INT
    DECLARE @CurrentMapX INT
    DECLARE @CurrentMapY INT
    DECLARE @Gender VARCHAR(10)
    DECLARE @Result TINYINT
    DECLARE @SuccessCount INT = 0
    DECLARE @FailCount INT = 0
    
    SET @Gender = CASE WHEN @UseFemaleName = 1 THEN 'Female' ELSE 'Male' END
    
    WHILE @i < @BotCount
    BEGIN
        -- Generate account name
        SET @AccountID = 'Bot' + RIGHT('0000' + CAST(@StartBotNumber + @i AS VARCHAR), 4)
        
        -- Get random name
        EXEC WZ_GetRandomBotName @Gender, @Language, @CharName OUTPUT
        
        -- Create full character name with number suffix
        SET @FullCharName = LEFT(@CharName, 8) + RIGHT('00' + CAST((ABS(CHECKSUM(NEWID())) % 90) + 10 AS VARCHAR), 2)
        
        -- Select class (random if -1)
        IF @ClassCode = -1
        BEGIN
            -- Random class selection based on distribution
            DECLARE @randClass INT = ABS(CHECKSUM(NEWID())) % 100
            SET @CurrentClass = CASE
                WHEN @randClass < 15 THEN 0   -- DW 15%
                WHEN @randClass < 45 THEN 16  -- DK 30%
                WHEN @randClass < 65 THEN 32  -- ELF 20%
                WHEN @randClass < 75 THEN 48  -- MG 10%
                WHEN @randClass < 85 THEN 64  -- DL 10%
                WHEN @randClass < 95 THEN 80  -- SUM 10%
                ELSE 96                       -- RF 5%
            END
        END
        ELSE
        BEGIN
            SET @CurrentClass = @ClassCode
        END
        
        -- Select config (random if -1)
        IF @ConfigIndex = -1
        BEGIN
            EXEC WZ_GetRandomConfigIndex @CurrentClass, 127, @CurrentConfig OUTPUT
        END
        ELSE
        BEGIN
            SET @CurrentConfig = @ConfigIndex
        END
        
        -- Random level within range
        SET @CurrentLevel = @MinLevel + (ABS(CHECKSUM(NEWID())) % (@MaxLevel - @MinLevel + 1))
        
        -- Position variation
        SET @CurrentMapX = @MapX + ((@i % 20) - 10)
        SET @CurrentMapY = @MapY + (((@i / 20) % 20) - 10)
        
        -- Create the bot
        BEGIN TRY
            EXEC WZ_CreateBot 
                @AccountID, @FullCharName, @CurrentClass, @CurrentConfig,
                @CurrentLevel, @MapNumber, @CurrentMapX, @CurrentMapY, 20000000
            
            SET @SuccessCount = @SuccessCount + 1
        END TRY
        BEGIN CATCH
            SET @FailCount = @FailCount + 1
            PRINT 'Failed to create bot: ' + @AccountID + ' - ' + ERROR_MESSAGE()
        END CATCH
        
        SET @i = @i + 1
    END
    
    -- Return summary
    SELECT 
        @BotCount AS TotalRequested,
        @SuccessCount AS SuccessCount,
        @FailCount AS FailCount
END
GO

-- =============================================
-- Utility: Reset Bot Names Usage
-- =============================================
IF EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[WZ_ResetBotNames]') AND type in (N'P', N'PC'))
    DROP PROCEDURE [dbo].[WZ_ResetBotNames]
GO

CREATE PROCEDURE [dbo].[WZ_ResetBotNames]
AS
BEGIN
    UPDATE BotNames SET IsUsed = 0
    PRINT 'All bot names reset to unused status'
END
GO

-- =============================================
-- Utility: Delete Bot Characters
-- =============================================
IF EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[WZ_DeleteBotRange]') AND type in (N'P', N'PC'))
    DROP PROCEDURE [dbo].[WZ_DeleteBotRange]
GO

CREATE PROCEDURE [dbo].[WZ_DeleteBotRange]
    @StartBotNumber INT,
    @EndBotNumber INT
AS
BEGIN
    SET NOCOUNT ON
    
    DECLARE @i INT = @StartBotNumber
    DECLARE @AccountID VARCHAR(10)
    DECLARE @DeleteCount INT = 0
    
    WHILE @i <= @EndBotNumber
    BEGIN
        SET @AccountID = 'Bot' + RIGHT('0000' + CAST(@i AS VARCHAR), 4)
        
        -- Delete character
        DELETE FROM Character WHERE AccountID = @AccountID
        
        -- Clear account character slots
        UPDATE AccountCharacter 
        SET GameID1 = NULL, GameID2 = NULL, GameID3 = NULL, GameID4 = NULL, GameID5 = NULL
        WHERE Id = @AccountID
        
        -- Optionally delete account (uncomment if needed)
        -- DELETE FROM MEMB_INFO WHERE memb___id = @AccountID
        
        SET @DeleteCount = @DeleteCount + @@ROWCOUNT
        SET @i = @i + 1
    END
    
    PRINT 'Deleted ' + CAST(@DeleteCount AS VARCHAR) + ' bot characters'
END
GO

PRINT 'All stored procedures created successfully!'
GO
