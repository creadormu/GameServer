// =============================================
// BotCreation_StoredProcedure.h
// Header file for bot creation functions
// =============================================

#ifndef BOT_CREATION_STORED_PROCEDURE_H
#define BOT_CREATION_STORED_PROCEDURE_H

#include <vector>
#include <string>


// =====================================================
// FUNCTION DECLARATIONS
// =====================================================

// Initialize ODBC connection for bot creation
bool InitializeBotODBC(const char* server, const char* database, const char* user, const char* password);

// Test SQL connection and retrieve database list
bool TestSQLConnection(const char* serverName, std::vector<std::string>& databases, char* errorMsg, int errorMsgSize);

// Call stored procedure to create a single bot
bool CallBotStoredProcedure(const char* accountID, const char* charName,
    int classCode, int level, int mapNumber,
    int mapX, int mapY, int str, int dex, int vit,
    int ene, int cmd, const char* invHex, const char* magicHex, const char* language);

// Batch create bots using stored procedure
bool CreateBotsViaBatch(int startFrom, int botCount, int minLevel, int maxLevel,
    int mapNumber, int mapX, int mapY);

// Main function: Create multiple bots with advanced configuration (Stored Procedure version)
bool CreateMultipleBotsAdvanced_StoredProc(int botCount, int startFrom, int gateNumber,
    int mapNumber, int mapX, int mapY,
    int minLevel, int maxLevel, int selectedClass,
    int phamViTrain, int moveRange, int timeReturn,
    int tuNhatItem, int tuDongReset, int partyMode,
    int pvpMode, int postKhiDie, int enabledConfigs);

// Clean bot accounts from database (accounts starting with "Bot")
bool CleanBotAccounts();

#endif // BOT_CREATION_STORED_PROCEDURE_H