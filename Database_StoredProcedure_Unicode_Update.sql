-- =============================================
-- Update Stored Procedures for Unicode Support
-- =============================================
-- This script updates common MuOnline stored procedures to use NVARCHAR
-- instead of VARCHAR for Unicode character support
--
-- IMPORTANT: Backup your stored procedures before running this!
-- =============================================

USE [MuOnline]
GO

PRINT '========================================='
PRINT 'Updating Stored Procedures for Unicode'
PRINT '========================================='
PRINT ''

-- =============================================
-- 1. Character Creation Procedure
-- =============================================
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'WZ_CreateCharacter')
BEGIN
    DROP PROCEDURE [dbo].[WZ_CreateCharacter]
    PRINT 'Dropped old WZ_CreateCharacter procedure'
END
GO

CREATE PROCEDURE [dbo].[WZ_CreateCharacter]
    @AccountID NVARCHAR(10),
    @Name NVARCHAR(10),
    @Class TINYINT,
    @Map SMALLINT,
    @PosX SMALLINT,
    @PosY SMALLINT,
    @Dir TINYINT,
    @Level INT,
    @Strength INT,
    @Dexterity INT,
    @Vitality INT,
    @Energy INT,
    @Leadership INT
AS
BEGIN
    SET NOCOUNT ON;
    
    -- Check if character name already exists
    IF EXISTS (SELECT 1 FROM [Character] WHERE [Name] = @Name)
    BEGIN
        SELECT -1 AS Result -- Character name already exists
        RETURN
    END
    
    -- Check if account has reached max characters
    DECLARE @CharCount INT
    SELECT @CharCount = COUNT(*) FROM [Character] WHERE [AccountID] = @AccountID
    
    IF @CharCount >= 5
    BEGIN
        SELECT -2 AS Result -- Max characters reached
        RETURN
    END
    
    -- Create character
    INSERT INTO [Character] (
        [AccountID], [Name], [Class], [cLevel], 
        [MapNumber], [MapPosX], [MapPosY], [MapDir],
        [Strength], [Dexterity], [Vitality], [Energy], [Leadership],
        [LevelUpPoint], [Experience], [Money]
    )
    VALUES (
        @AccountID, @Name, @Class, @Level,
        @Map, @PosX, @PosY, @Dir,
        @Strength, @Dexterity, @Vitality, @Energy, @Leadership,
        0, 0, 0
    )
    
    -- Add to AccountCharacter list
    IF EXISTS (SELECT 1 FROM [AccountCharacter] WHERE [Id] = @AccountID)
    BEGIN
        -- Update existing slots
        IF (SELECT [GameID1] FROM [AccountCharacter] WHERE [Id] = @AccountID) IS NULL
            UPDATE [AccountCharacter] SET [GameID1] = @Name WHERE [Id] = @AccountID
        ELSE IF (SELECT [GameID2] FROM [AccountCharacter] WHERE [Id] = @AccountID) IS NULL
            UPDATE [AccountCharacter] SET [GameID2] = @Name WHERE [Id] = @AccountID
        ELSE IF (SELECT [GameID3] FROM [AccountCharacter] WHERE [Id] = @AccountID) IS NULL
            UPDATE [AccountCharacter] SET [GameID3] = @Name WHERE [Id] = @AccountID
        ELSE IF (SELECT [GameID4] FROM [AccountCharacter] WHERE [Id] = @AccountID) IS NULL
            UPDATE [AccountCharacter] SET [GameID4] = @Name WHERE [Id] = @AccountID
        ELSE IF (SELECT [GameID5] FROM [AccountCharacter] WHERE [Id] = @AccountID) IS NULL
            UPDATE [AccountCharacter] SET [GameID5] = @Name WHERE [Id] = @AccountID
    END
    ELSE
    BEGIN
        -- Create new AccountCharacter entry
        INSERT INTO [AccountCharacter] ([Id], [GameID1])
        VALUES (@AccountID, @Name)
    END
    
    SELECT 0 AS Result -- Success
END
GO

PRINT 'Created WZ_CreateCharacter with Unicode support'
PRINT ''

-- =============================================
-- 2. Character Delete Procedure
-- =============================================
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'WZ_DeleteCharacter')
BEGIN
    DROP PROCEDURE [dbo].[WZ_DeleteCharacter]
    PRINT 'Dropped old WZ_DeleteCharacter procedure'
END
GO

CREATE PROCEDURE [dbo].[WZ_DeleteCharacter]
    @AccountID NVARCHAR(10),
    @Name NVARCHAR(10)
AS
BEGIN
    SET NOCOUNT ON;
    
    -- Check if character exists and belongs to account
    IF NOT EXISTS (SELECT 1 FROM [Character] WHERE [AccountID] = @AccountID AND [Name] = @Name)
    BEGIN
        SELECT -1 AS Result -- Character not found or doesn't belong to account
        RETURN
    END
    
    -- Delete character
    DELETE FROM [Character] WHERE [AccountID] = @AccountID AND [Name] = @Name
    
    -- Update AccountCharacter list
    UPDATE [AccountCharacter] 
    SET [GameID1] = CASE WHEN [GameID1] = @Name THEN NULL ELSE [GameID1] END,
        [GameID2] = CASE WHEN [GameID2] = @Name THEN NULL ELSE [GameID2] END,
        [GameID3] = CASE WHEN [GameID3] = @Name THEN NULL ELSE [GameID3] END,
        [GameID4] = CASE WHEN [GameID4] = @Name THEN NULL ELSE [GameID4] END,
        [GameID5] = CASE WHEN [GameID5] = @Name THEN NULL ELSE [GameID5] END
    WHERE [Id] = @AccountID
    
    SELECT 0 AS Result -- Success
END
GO

PRINT 'Created WZ_DeleteCharacter with Unicode support'
PRINT ''

-- =============================================
-- 3. Guild Create Procedure
-- =============================================
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'WZ_CreateGuild')
BEGIN
    DROP PROCEDURE [dbo].[WZ_CreateGuild]
    PRINT 'Dropped old WZ_CreateGuild procedure'
END
GO

CREATE PROCEDURE [dbo].[WZ_CreateGuild]
    @GuildName NVARCHAR(10),
    @MasterName NVARCHAR(10),
    @GuildMark VARBINARY(32)
AS
BEGIN
    SET NOCOUNT ON;
    
    -- Check if guild name already exists
    IF EXISTS (SELECT 1 FROM [Guild] WHERE [G_Name] = @GuildName)
    BEGIN
        SELECT -1 AS Result -- Guild name already exists
        RETURN
    END
    
    -- Create guild
    INSERT INTO [Guild] ([G_Name], [G_Master], [G_Mark])
    VALUES (@GuildName, @MasterName, @GuildMark)
    
    -- Get guild number
    DECLARE @GuildNumber INT
    SELECT @GuildNumber = [G_Num] FROM [Guild] WHERE [G_Name] = @GuildName
    
    -- Add master to guild members
    INSERT INTO [GuildMember] ([G_Num], [Name], [G_Status])
    VALUES (@GuildNumber, @MasterName, 0x80) -- 0x80 = Guild Master
    
    SELECT @GuildNumber AS Result -- Return guild number
END
GO

PRINT 'Created WZ_CreateGuild with Unicode support'
PRINT ''

-- =============================================
-- 4. Bot Creation Procedure (if you use bots)
-- =============================================
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'CreateBotCharacter')
BEGIN
    DROP PROCEDURE [dbo].[CreateBotCharacter]
    PRINT 'Dropped old CreateBotCharacter procedure'
END
GO

CREATE PROCEDURE [dbo].[CreateBotCharacter]
    @AccountID NVARCHAR(10),
    @CharName NVARCHAR(10),
    @ClassCode INT,
    @Level INT,
    @MapNumber INT,
    @MapX INT,
    @MapY INT,
    @Str INT,
    @Dex INT,
    @Vit INT,
    @Ene INT,
    @Cmd INT,
    @InvHex NVARCHAR(MAX),
    @MagicHex NVARCHAR(MAX),
    @Language NVARCHAR(20) = 'English'
AS
BEGIN
    SET NOCOUNT ON;
    
    BEGIN TRY
        BEGIN TRANSACTION
        
        -- Check if account exists, if not create it
        IF NOT EXISTS (SELECT 1 FROM [MEMB_INFO] WHERE [memb___id] = @AccountID)
        BEGIN
            INSERT INTO [MEMB_INFO] (
                [memb___id], [memb__pwd], [memb_name], 
                [sno__numb], [bloc_code], [ctl1_code]
            )
            VALUES (
                @AccountID, '0000', @AccountID,
                '000000000000000000', '0', '0'
            )
        END
        
        -- Create character
        INSERT INTO [Character] (
            [AccountID], [Name], [Class], [cLevel],
            [MapNumber], [MapPosX], [MapPosY],
            [Strength], [Dexterity], [Vitality], [Energy], [Leadership],
            [Inventory], [MagicList], [Language]
        )
        VALUES (
            @AccountID, @CharName, @ClassCode, @Level,
            @MapNumber, @MapX, @MapY,
            @Str, @Dex, @Vit, @Ene, @Cmd,
            CONVERT(VARBINARY(MAX), @InvHex, 2),
            CONVERT(VARBINARY(MAX), @MagicHex, 2),
            @Language
        )
        
        -- Add to AccountCharacter if needed
        IF NOT EXISTS (SELECT 1 FROM [AccountCharacter] WHERE [Id] = @AccountID)
        BEGIN
            INSERT INTO [AccountCharacter] ([Id], [GameID1])
            VALUES (@AccountID, @CharName)
        END
        
        COMMIT TRANSACTION
        SELECT 1 AS Result -- Success
    END TRY
    BEGIN CATCH
        ROLLBACK TRANSACTION
        SELECT -1 AS Result -- Error
    END CATCH
END
GO

PRINT 'Created CreateBotCharacter with Unicode support'
PRINT ''

-- =============================================
-- 5. Character Rename Procedure
-- =============================================
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'WZ_RenameCharacter')
BEGIN
    DROP PROCEDURE [dbo].[WZ_RenameCharacter]
    PRINT 'Dropped old WZ_RenameCharacter procedure'
END
GO

CREATE PROCEDURE [dbo].[WZ_RenameCharacter]
    @OldName NVARCHAR(10),
    @NewName NVARCHAR(10),
    @AccountID NVARCHAR(10)
AS
BEGIN
    SET NOCOUNT ON;
    
    -- Check if new name already exists
    IF EXISTS (SELECT 1 FROM [Character] WHERE [Name] = @NewName)
    BEGIN
        SELECT -1 AS Result -- New name already exists
        RETURN
    END
    
    -- Check if old character exists and belongs to account
    IF NOT EXISTS (SELECT 1 FROM [Character] WHERE [Name] = @OldName AND [AccountID] = @AccountID)
    BEGIN
        SELECT -2 AS Result -- Character not found or doesn't belong to account
        RETURN
    END
    
    BEGIN TRANSACTION
    
    -- Update character name
    UPDATE [Character] SET [Name] = @NewName WHERE [Name] = @OldName AND [AccountID] = @AccountID
    
    -- Update AccountCharacter list
    UPDATE [AccountCharacter]
    SET [GameID1] = CASE WHEN [GameID1] = @OldName THEN @NewName ELSE [GameID1] END,
        [GameID2] = CASE WHEN [GameID2] = @OldName THEN @NewName ELSE [GameID2] END,
        [GameID3] = CASE WHEN [GameID3] = @OldName THEN @NewName ELSE [GameID3] END,
        [GameID4] = CASE WHEN [GameID4] = @OldName THEN @NewName ELSE [GameID4] END,
        [GameID5] = CASE WHEN [GameID5] = @OldName THEN @NewName ELSE [GameID5] END
    WHERE [Id] = @AccountID
    
    -- Update guild member if character is in guild
    IF EXISTS (SELECT 1 FROM [GuildMember] WHERE [Name] = @OldName)
    BEGIN
        UPDATE [GuildMember] SET [Name] = @NewName WHERE [Name] = @OldName
    END
    
    -- Update guild master if character is guild master
    IF EXISTS (SELECT 1 FROM [Guild] WHERE [G_Master] = @OldName)
    BEGIN
        UPDATE [Guild] SET [G_Master] = @NewName WHERE [G_Master] = @OldName
    END
    
    COMMIT TRANSACTION
    
    SELECT 0 AS Result -- Success
END
GO

PRINT 'Created WZ_RenameCharacter with Unicode support'
PRINT ''

PRINT '========================================='
PRINT 'Stored Procedures Updated Successfully!'
PRINT '========================================='
PRINT ''
PRINT 'Updated Procedures:'
PRINT '  - WZ_CreateCharacter'
PRINT '  - WZ_DeleteCharacter'
PRINT '  - WZ_CreateGuild'
PRINT '  - CreateBotCharacter'
PRINT '  - WZ_RenameCharacter'
PRINT ''
PRINT 'Note: Update any other custom stored procedures that use VARCHAR'
PRINT 'to use NVARCHAR for proper Unicode support.'
PRINT ''

GO
