// =============================================
// COMPLETE FIXED VERSION - Bot Creation via Stored Procedure
// =============================================

#include "stdafx.h"
#include "GameServer.h"
#include "Util.h"
#include "User.h"
#include "ClassConfig.h"
#include "NameManager.h"
#include <sql.h>
#include <sqlext.h>
#include <vector>
#include <string>
#include <map>

#pragma comment(lib, "odbc32.lib")
#pragma comment(lib, "odbccp32.lib")

// =============================================
// GLOBAL ODBC HANDLES
// =============================================
SQLHENV g_hOdbcEnv = SQL_NULL_HENV;
SQLHDBC g_hOdbcConn = SQL_NULL_HDBC;
bool g_OdbcInitialized = false;

// =====================================================
// 1. Initialize ODBC Connection
// =====================================================
bool InitializeBotODBC(const char* server, const char* database, const char* user, const char* password)
{
    if (g_OdbcInitialized)
    {
        LogAdd(LOG_BLUE, (char*)"[BotODBC] Already initialized");
        return true;
    }

    SQLRETURN ret;

    // Allocate environment handle
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &g_hOdbcEnv);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
    {
        LogAdd(LOG_RED, (char*)"[BotODBC] Failed to allocate environment handle");
        return false;
    }

    // Set ODBC version
    SQLSetEnvAttr(g_hOdbcEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

    // Allocate connection handle
    ret = SQLAllocHandle(SQL_HANDLE_DBC, g_hOdbcEnv, &g_hOdbcConn);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
    {
        LogAdd(LOG_RED, (char*)"[BotODBC] Failed to allocate connection handle");
        SQLFreeHandle(SQL_HANDLE_ENV, g_hOdbcEnv);
        return false;
    }

    // Build connection string (using Windows Authentication)
    char connStr[512];
    sprintf_s(connStr, sizeof(connStr),
        "DRIVER={SQL Server};SERVER=%s;DATABASE=%s;Trusted_Connection=yes;",
        server, database);

    // Connect to database
    ret = SQLDriverConnect(g_hOdbcConn, NULL, (SQLCHAR*)connStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
    {
        LogAdd(LOG_GREEN, (char*)"[BotODBC] Connected to database successfully");
        g_OdbcInitialized = true;
        return true;
    }
    else
    {
        SQLCHAR sqlState[6], errorMsg[SQL_MAX_MESSAGE_LENGTH];
        SQLINTEGER nativeError;
        SQLSMALLINT msgLen;

        SQLGetDiagRec(SQL_HANDLE_DBC, g_hOdbcConn, 1, sqlState, &nativeError,
            errorMsg, sizeof(errorMsg), &msgLen);

        LogAdd(LOG_RED, (char*)"[BotODBC] Connection failed: %s", errorMsg);

        SQLFreeHandle(SQL_HANDLE_DBC, g_hOdbcConn);
        SQLFreeHandle(SQL_HANDLE_ENV, g_hOdbcEnv);
        g_hOdbcConn = SQL_NULL_HDBC;
        g_hOdbcEnv = SQL_NULL_HENV;

        return false;
    }
}

// =====================================================
// 2. Test SQL Connection and Get Database List
// =====================================================
bool TestSQLConnection(const char* serverName, std::vector<std::string>& databases, char* errorMsg, int errorMsgSize)
{
    SQLHENV hEnv = SQL_NULL_HENV;
    SQLHDBC hDbc = SQL_NULL_HDBC;
    SQLHSTMT hStmt = SQL_NULL_HSTMT;
    SQLRETURN ret;

    databases.clear();

    // Allocate environment
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
    {
        sprintf_s(errorMsg, errorMsgSize, "Failed to allocate environment handle");
        return false;
    }

    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

    // Allocate connection
    ret = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
    {
        sprintf_s(errorMsg, errorMsgSize, "Failed to allocate connection handle");
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
        return false;
    }

    // Build connection string (Windows Authentication to master)
    char connStr[512];
    sprintf_s(connStr, sizeof(connStr),
        "DRIVER={SQL Server};SERVER=%s;DATABASE=master;Trusted_Connection=yes;",
        serverName);

    // Try to connect
    ret = SQLDriverConnect(hDbc, NULL, (SQLCHAR*)connStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
    {
        SQLCHAR sqlState[6], dbErrorMsg[SQL_MAX_MESSAGE_LENGTH];
        SQLINTEGER nativeError;
        SQLSMALLINT msgLen;

        SQLGetDiagRec(SQL_HANDLE_DBC, hDbc, 1, sqlState, &nativeError,
            dbErrorMsg, sizeof(dbErrorMsg), &msgLen);

        sprintf_s(errorMsg, errorMsgSize,
            "Connection failed!\n\nServer: %s\nError: %s\n\nMake sure:\n- SQL Server is running\n- Server name is correct\n- Windows Authentication is enabled",
            serverName, dbErrorMsg);

        SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
        return false;
    }

    // Get database list
    ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
    {
        const char* query = "SELECT name FROM sys.databases WHERE name NOT IN ('master','tempdb','model','msdb') ORDER BY name";
        ret = SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS);

        if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
        {
            char dbName[128];
            SQLLEN indicator;

            while (SQLFetch(hStmt) == SQL_SUCCESS)
            {
                SQLGetData(hStmt, 1, SQL_C_CHAR, dbName, sizeof(dbName), &indicator);
                databases.push_back(std::string(dbName));
            }
        }

        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    }

    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);

    return true;
}

// =====================================================
// 3. Call Stored Procedure for Single Bot
// =====================================================
bool CallBotStoredProcedure(const char* accountID, const char* charName,
    int classCode, int level, int mapNumber,
    int mapX, int mapY, int str, int dex, int vit,
    int ene, int cmd, const char* invHex, const char* magicHex)
{
    // Auto-initialize if needed
    if (!g_OdbcInitialized)
    {
        if (!InitializeBotODBC(".\\SQLEXPRESS", "MuOnline", "", ""))
        {
            LogAdd(LOG_RED, (char*)"[CallStoredProc] ODBC not initialized!");
            return false;
        }
    }

    SQLHSTMT hStmt = NULL;
    SQLRETURN ret;

    ret = SQLAllocHandle(SQL_HANDLE_STMT, g_hOdbcConn, &hStmt);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
    {
        LogAdd(LOG_RED, (char*)"[StoredProc] Failed to allocate statement");
        return false;
    }

    // Build stored procedure call
    char query[8192];
    sprintf_s(query, sizeof(query),
        "EXEC WZ_CreateBotDirect '%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', '%s'",
        accountID, charName, classCode, level, mapNumber, mapX, mapY,
        str, dex, vit, ene, cmd, invHex, magicHex);

    // Execute
    ret = SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS);

    bool success = (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);

    if (!success)
    {
        SQLCHAR sqlState[6], errorMsg[SQL_MAX_MESSAGE_LENGTH];
        SQLINTEGER nativeError;
        SQLSMALLINT msgLen;

        if (SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, 1, sqlState, &nativeError,
            errorMsg, sizeof(errorMsg), &msgLen) == SQL_SUCCESS)
        {
            LogAdd(LOG_RED, (char*)"[StoredProc] Error: %s", errorMsg);
        }
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return success;
}

// =====================================================
// 4. Batch Creation Using Stored Procedure
// =====================================================
bool CreateBotsViaBatch(int startFrom, int botCount, int minLevel, int maxLevel,
    int mapNumber, int mapX, int mapY)
{
    if (!g_OdbcInitialized)
    {
        LogAdd(LOG_RED, (char*)"[BatchCreate] ODBC not initialized!");
        return false;
    }

    SQLHSTMT hStmt = NULL;
    SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, g_hOdbcConn, &hStmt);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
    {
        LogAdd(LOG_RED, (char*)"[BatchCreate] Failed to allocate statement");
        return false;
    }

    char query[512];
    sprintf_s(query, sizeof(query),
        "EXEC WZ_CreateBotBatch %d, %d, -1, -1, %d, %d, %d, %d, %d, 0, NULL",
        startFrom, botCount, minLevel, maxLevel, mapNumber, mapX, mapY);

    LogAdd(LOG_BLACK, (char*)"[BatchCreate] Creating %d bots via stored procedure...", botCount);

    ret = SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS);

    bool success = (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);

    if (success)
    {
        LogAdd(LOG_GREEN, (char*)"[BatchCreate] %d bots created successfully!", botCount);
    }
    else
    {
        SQLCHAR errorMsg[SQL_MAX_MESSAGE_LENGTH];
        SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, 1, NULL, NULL, errorMsg, sizeof(errorMsg), NULL);
        LogAdd(LOG_RED, (char*)"[BatchCreate] Failed: %s", errorMsg);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return success;
}

// =====================================================
// 5. MAIN FUNCTION - Create Multiple Bots (Stored Procedure Version)
// =====================================================
bool CreateMultipleBotsAdvanced_StoredProc(int botCount, int startFrom, int gateNumber, int mapNumber, int mapX, int mapY,
    int minLevel, int maxLevel, int selectedClass, int phamViTrain, int moveRange, int timeReturn,
    int tuNhatItem, int tuDongReset, int partyMode, int pvpMode, int postKhiDie, int enabledConfigs)
{
    LogAdd(LOG_BLACK, (char*)"[CreateBots] ===== START (Stored Procedure Version) =====");
    LogAdd(LOG_BLACK, (char*)"[CreateBots] Count=%d, StartFrom=%d, EnabledConfigs=%d", botCount, startFrom, enabledConfigs);

    // SAFETY CHECKS
    if (botCount > 1000 || botCount < 1)
    {
        LogAdd(LOG_RED, (char*)"[CreateBots] ERROR: Invalid bot count");
        return false;
    }

    if (startFrom < 1)
    {
        LogAdd(LOG_RED, (char*)"[CreateBots] ERROR: Invalid start number");
        return false;
    }

    // Auto-initialize ODBC if not already done
    if (!g_OdbcInitialized)
    {
        LogAdd(LOG_BLUE, (char*)"[CreateBots] Initializing ODBC connection...");
        if (!InitializeBotODBC(".\\SQLEXPRESS", "MuOnline", "", ""))
        {
            LogAdd(LOG_RED, (char*)"[CreateBots] FAILED to initialize ODBC! Cannot proceed.");
            return false;
        }
    }

    // Class configuration
    struct ClassConfig {
        int classCode;
        const char* className;
        int mainSkill;
        int secondarySkill;
        int buff1, buff2, buff3;
        int percentage;
        int str, dex, vit, ene, cmd;
        bool useFemaleNames;
    };

    ClassConfig classes[] = {
        {0,  "DW",  9,   12,  16,  -1,  -1, 15, 2000, 2000, 2000, 5000, 0,    false},
        {16, "DK",  44,  41,  48,  -1,  -1, 30, 5000, 4500, 5500, 4000, 0,    false},
        {32, "ELF", 24,  52,  26,  27,  28, 20, 2500, 4500, 2500, 3000, 0,    true},
        {48, "MG",  8,  55,   -1,  -1,  -1, 10, 4000, 3000, 4000, 4000, 0,    false},
        {64, "DL",  65,  61,  64,  -1,  -1, 10, 4500, 3500, 4500, 2500, 5000, false},
        {80, "SUM", 214, 215, 217, 218, -1, 10, 2000, 2000, 2000, 5000, 0,    true},
        {96, "RF",  264, 263, 266, 268, -1, 5,  4500, 4500, 4500, 2000, 0,    false}
    };

    int classCount = sizeof(classes) / sizeof(classes[0]);

    // Calculate cumulative percentages
    int cumulativePercentages[10];
    int total = 0;
    for (int i = 0; i < classCount; i++)
    {
        total += classes[i].percentage;
        cumulativePercentages[i] = total;
    }

    LogAdd(LOG_BLACK, (char*)"[CreateBots] Opening XML file...");

    // Open XML file
    FILE* xmlFile = NULL;
    errno_t err = fopen_s(&xmlFile, "IA\\Generated\\IA_Accounts.xml", "w");
    if (err != 0 || !xmlFile)
    {
        LogAdd(LOG_RED, (char*)"[CreateBots] FAILED to open XML file! Error: %d", err);
        return false;
    }
    LogAdd(LOG_GREEN, (char*)"[CreateBots] XML file opened successfully");

    // Write XML header
    fputs("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n\n", xmlFile);
    fputs("<MSGThongBao IndexMesMin=\"3020\" IndexMesMax=\"3030\"/>\n\n", xmlFile);
    fputs("<Config DelayRange=\"40000\" />\n\n", xmlFile);
    fputs("<FakeOnlineData>\n", xmlFile);

    LogAdd(LOG_GREEN, (char*)"[CreateBots] Starting bot generation...");

    // Process bots
    int successCount = 0;
    int failCount = 0;

    for (int i = 0; i < botCount; i++)
    {
        int botNumber = startFrom + i;
        char account[11];
        char charName[11];

        sprintf_s(account, sizeof(account), "Bot%04d", botNumber);

        // Select class
        ClassConfig* selectedClassConfig = NULL;

        if (selectedClass == -1)
        {
            int randValue = i % 100;
            for (int j = 0; j < classCount; j++)
            {
                if (randValue < cumulativePercentages[j])
                {
                    selectedClassConfig = &classes[j];
                    break;
                }
            }
        }
        else
        {
            for (int j = 0; j < classCount; j++)
            {
                if (classes[j].classCode == selectedClass)
                {
                    selectedClassConfig = &classes[j];
                    break;
                }
            }
        }

        if (!selectedClassConfig) selectedClassConfig = &classes[0];

        // Select random config
        int configIndex = g_ClassConfigManager.GetRandomConfigIndex(enabledConfigs);

        // Verify class is configured
        if (!g_ClassConfigManager.IsClassConfigured(selectedClassConfig->classCode, configIndex))
        {
            LogAdd(LOG_RED, (char*)"[CreateBots] Class %s NOT configured in Config %d!",
                selectedClassConfig->className, configIndex);
            fclose(xmlFile);
            return false;
        }

        // Generate name
        if (selectedClassConfig->useFemaleNames)
        {
            sprintf_s(charName, sizeof(charName), "%s%d",
                g_NameManager.GetRandomFemaleName(), GetLargeRand() % 90 + 10);
        }
        else
        {
            sprintf_s(charName, sizeof(charName), "%s%d",
                g_NameManager.GetRandomMaleName(), GetLargeRand() % 90 + 10);
        }

        int level = minLevel + (GetLargeRand() % (maxLevel - minLevel + 1));
        int finalMapX = mapX + ((i % 20) - 10);
        int finalMapY = mapY + (((i / 20) % 20) - 10);

        // Write to XML
        fprintf(xmlFile,
            "  <Info Account=\"%s\" Password=\"123456\" Name=\"%s\" "
            "SkillID=\"%d\" SecondarySkillID=\"%d\" "
            "UseBuffs_0=\"%d\" UseBuffs_1=\"%d\" UseBuffs_2=\"%d\" "
            "GateNumber=\"%d\" Map=\"%d\" MapX=\"%d\" MapY=\"%d\" "
            "PhamViTrain=\"%d\" MoveRange=\"%d\" TimeReturn=\"%d\" "
            "TuNhatItem=\"%d\" TuDongReset=\"%d\" "
            "PartyMode=\"%d\" PVPMode=\"%d\" PostKhiDie=\"%d\" />\n",
            account, charName,
            selectedClassConfig->mainSkill, selectedClassConfig->secondarySkill,
            selectedClassConfig->buff1, selectedClassConfig->buff2, selectedClassConfig->buff3,
            gateNumber, mapNumber, finalMapX, finalMapY,
            phamViTrain, moveRange, timeReturn,
            tuNhatItem, tuDongReset,
            partyMode, pvpMode, postKhiDie
        );

        // Get hex data
        const char* invHex = g_ClassConfigManager.GetInventoryHex(selectedClassConfig->classCode, configIndex);
        const char* magicHex = g_ClassConfigManager.GetMagicListHex(selectedClassConfig->classCode, configIndex);

        if (!invHex || !magicHex)
        {
            LogAdd(LOG_RED, (char*)"[CreateBots] Bot %d: Failed to get hex data", i + 1);
            failCount++;
            continue;
        }

        // Call stored procedure to create bot in database
        if (CallBotStoredProcedure(account, charName, selectedClassConfig->classCode,
            level, mapNumber, finalMapX, finalMapY,
            selectedClassConfig->str, selectedClassConfig->dex,
            selectedClassConfig->vit, selectedClassConfig->ene,
            selectedClassConfig->cmd, invHex, magicHex))
        {
            successCount++;
            if ((i + 1) % 10 == 0)
            {
                LogAdd(LOG_BLUE, (char*)"[CreateBots] Progress: %d/%d", i + 1, botCount);
            }
        }
        else
        {
            failCount++;
            LogAdd(LOG_RED, (char*)"[CreateBots] Bot %d (%s) creation failed", i + 1, account);
        }
    }

    // Write XML footer
    fputs("</FakeOnlineData>\n", xmlFile);
    fclose(xmlFile);

    LogAdd(LOG_GREEN, (char*)"[CreateBots] ===== COMPLETED =====");
    LogAdd(LOG_GREEN, (char*)"[CreateBots] Success: %d, Failed: %d", successCount, failCount);
    LogAdd(LOG_GREEN, (char*)"[CreateBots] XML file: IA\\Generated\\IA_Accounts.xml");

    return (successCount > 0);
}

// =====================================================
// 6. Clean Bot Accounts Function
// =====================================================
bool CleanBotAccounts()
{
    if (!g_OdbcInitialized || g_hOdbcConn == SQL_NULL_HDBC)
    {
        LogAdd(LOG_RED, (char*)"[CleanBots] Database not connected!");
        return false;
    }

    SQLHSTMT hStmt = SQL_NULL_HSTMT;
    SQLRETURN ret;

    LogAdd(LOG_BLACK, (char*)"[CleanBots] ===== START CLEANING BOT ACCOUNTS =====");

    // Allocate statement handle
    ret = SQLAllocHandle(SQL_HANDLE_STMT, g_hOdbcConn, &hStmt);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
    {
        LogAdd(LOG_RED, (char*)"[CleanBots] Failed to allocate statement handle");
        return false;
    }

    // Array of SQL queries to clean bot accounts
    const char* queries[] = {
        "DELETE FROM MEMB_INFO WHERE memb___id LIKE 'Bot%'",
        "DELETE FROM CHARACTER WHERE AccountID LIKE 'Bot%'",
        "DELETE FROM MEMB_STAT WHERE memb___id LIKE 'Bot%'",
        "DELETE FROM warehouse WHERE AccountID LIKE 'Bot%'",
        "DELETE FROM ExtWareHouse WHERE AccountID LIKE 'Bot%'",
        "DELETE FROM GuildMember WHERE Name IN (SELECT Name FROM CHARACTER WHERE AccountID LIKE 'Bot%')",
        "DELETE FROM AccountCharacter WHERE Id LIKE 'Bot%'",
        "DELETE FROM GameServerInfo WHERE AccountID LIKE 'Bot%'",
        "DELETE FROM MuCastle_MONEY_STATISTICS WHERE AccountID LIKE 'Bot%'",
        "DELETE FROM T_FriendList WHERE GUID IN (SELECT GUID FROM T_FriendMain WHERE Name IN (SELECT Name FROM CHARACTER WHERE AccountID LIKE 'Bot%'))",
        "DELETE FROM T_FriendMail WHERE GUID IN (SELECT GUID FROM T_FriendMain WHERE Name IN (SELECT Name FROM CHARACTER WHERE AccountID LIKE 'Bot%'))",
        "DELETE FROM T_FriendMain WHERE Name IN (SELECT Name FROM CHARACTER WHERE AccountID LIKE 'Bot%')",
        "DELETE FROM T_PetItem_Info WHERE AccountID LIKE 'Bot%'",
        "DELETE FROM T_WaitFriend WHERE FriendName IN (SELECT Name FROM CHARACTER WHERE AccountID LIKE 'Bot%')",
        "UPDATE MasterSkillTree SET MasterSkill = 0 WHERE Name IN (SELECT Name FROM CHARACTER WHERE AccountID LIKE 'Bot%')",
        NULL
    };

    int successCount = 0;
    int failCount = 0;

    // Execute each query
    for (int i = 0; queries[i] != NULL; i++)
    {
        LogAdd(LOG_BLUE, (char*)"[CleanBots] Executing query %d...", i + 1);

        ret = SQLExecDirect(hStmt, (SQLCHAR*)queries[i], SQL_NTS);

        if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
        {
            SQLLEN rowCount = 0;
            SQLRowCount(hStmt, &rowCount);
            LogAdd(LOG_GREEN, (char*)"[CleanBots] Query %d succeeded: %d rows affected", i + 1, (int)rowCount);
            successCount++;
        }
        else
        {
            SQLCHAR sqlState[6], errorMsg[SQL_MAX_MESSAGE_LENGTH];
            SQLINTEGER nativeError;
            SQLSMALLINT msgLen;

            SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, 1, sqlState, &nativeError,
                errorMsg, sizeof(errorMsg), &msgLen);

            LogAdd(LOG_RED, (char*)"[CleanBots] Query %d failed: %s", i + 1, errorMsg);
            failCount++;
        }

        // Close cursor for next query
        SQLCloseCursor(hStmt);
    }

    // Free statement handle
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

    LogAdd(LOG_GREEN, (char*)"[CleanBots] ===== COMPLETED =====");
    LogAdd(LOG_GREEN, (char*)"[CleanBots] Success: %d queries, Failed: %d queries", successCount, failCount);

    return (failCount == 0);
}