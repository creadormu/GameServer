USE [MuOnline]
GO

-- =============================================
-- Helper Procedure: Create Bot with Direct Parameters
-- =============================================
-- This version accepts hex data directly from C++ code
-- Use when you already have the stats and hex values
-- =============================================

IF EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[WZ_CreateBotDirect]') AND type in (N'P', N'PC'))
    DROP PROCEDURE [dbo].[WZ_CreateBotDirect]
GO

CREATE PROCEDURE [dbo].[WZ_CreateBotDirect]
    @AccountID VARCHAR(10),
    @CharName VARCHAR(10),
    @ClassCode INT,
    @Level INT,
    @MapNumber INT,
    @MapPosX INT,
    @MapPosY INT,
    @Strength INT,
    @Dexterity INT,
    @Vitality INT,
    @Energy INT,
    @Leadership INT,
    @InventoryHex VARCHAR(MAX),
    @MagicListHex VARCHAR(MAX),
    @Money INT = 20000000
AS
BEGIN
    SET NOCOUNT ON
    SET XACT_ABORT ON
    
    DECLARE @Result TINYINT
    SET @Result = 0x00
    
    -- Check if character already exists
    IF EXISTS (SELECT Name FROM Character WHERE Name = @CharName)
    BEGIN
        SET @Result = 0x01  -- Character name already exists
        RETURN 0x01
    END
    
    BEGIN TRAN
    
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
        GETDATE(), GETDATE(), 
        0x1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F1F, 
        0
    )
    
    SET @Result = @@ERROR
    
TranProcEnd:
    IF (@Result <> 0)
        ROLLBACK TRAN
    ELSE
        COMMIT TRAN

    SET NOCOUNT OFF
    SET XACT_ABORT OFF
    
    RETURN @Result
END
GO

PRINT 'WZ_CreateBotDirect created successfully'
PRINT 'This procedure accepts direct parameters from C++ code'
GO
