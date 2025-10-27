// =============================================
// FINAL VERSION - Bot Creation with SQL Names
// =============================================

#include "stdafx.h"
#include "GameServer.h"
#include "Util.h"
#include "ClassConfig.h"
#include <sql.h>
#include <sqlext.h>

#pragma comment(lib, "odbc32.lib")

// Global ODBC handles
extern SQLHENV g_hOdbcEnv;
extern SQLHDBC g_hOdbcConn;
extern bool g_OdbcInitialized;

// =====================================================
// Get Random Name from SQL Database
// =====================================================
bool GetRandomBotNameFromSQL(const char* gender, const char* language, char* outName, int outNameSize)
{
    if (!g_OdbcInitialized)
    {
        LogAdd(LOG_RED, (char*)"[GetName] ODBC not initialized");
        return false;
    }

    SQLHSTMT hStmt = NULL;
    SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, g_hOdbcConn, &hStmt);
    
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
    {
        LogAdd(LOG_RED, (char*)"[GetName] Failed to allocate statement");
        return false;
    }

    // Call stored procedure
    char query[256];
    if (language && strlen(language) > 0)
    {
        sprintf_s(query, sizeof(query),
            "DECLARE @name VARCHAR(50); EXEC WZ_GetRandomBotName '%s', '%s', 8, @name OUTPUT; SELECT @name",
            gender, language);
    }
    else
    {
        sprintf_s(query, sizeof(query),
            "DECLARE @name VARCHAR(50); EXEC WZ_GetRandomBotName '%s', NULL, 8, @name OUTPUT; SELECT @name",
            gender);
    }

    ret = SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS);
    
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
    {
        ret = SQLFetch(hStmt);
        if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
        {
            char tempName[128];
            SQLLEN indicator;
            SQLGetData(hStmt, 1, SQL_C_CHAR, tempName, sizeof(tempName), &indicator);
            
            // Truncate to 8 chars max (we'll add 2 digits later)
            if (strlen(tempName) > 8)
                tempName[8] = '\0';
                
            strcpy_s(outName, outNameSize, tempName);
            
            SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
            return true;
        }
    }
    
    // Fallback
    strcpy_s(outName, outNameSize, gender[0] == 'M' ? "Bot" : "Bot");
    
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return false;
}

// =====================================================
// Call Stored Procedure to Create Bot
// =====================================================
bool CallBotStoredProcedure(const char* accountID, const char* charName,
    int classCode, int level, int mapNumber,
    int mapX, int mapY, int str, int dex, int vit,
    int ene, int cmd, const char* invHex, const char* magicHex)
{
    if (!g_OdbcInitialized)
    {
        LogAdd(LOG_RED, (char*)"[CreateBot] ODBC not initialized");
        return false;
    }

    SQLHSTMT hStmt = NULL;
    SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, g_hOdbcConn, &hStmt);
    
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
    {
        LogAdd(LOG_RED, (char*)"[CreateBot] Failed to allocate statement");
        return false;
    }

    // Build stored procedure call
    char query[8192];
    sprintf_s(query, sizeof(query),
        "EXEC WZ_CreateBotDirect '%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', '%s'",
        accountID, charName, classCode, level, mapNumber, mapX, mapY,
        str, dex, vit, ene, cmd, invHex, magicHex);

    ret = SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS);
    
    bool success = (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);
    
    if (!success)
    {
        SQLCHAR errorMsg[SQL_MAX_MESSAGE_LENGTH];
        SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, 1, NULL, NULL, errorMsg, sizeof(errorMsg), NULL);
        LogAdd(LOG_RED, (char*)"[CreateBot] Error: %s", errorMsg);
    }
    
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return success;
}

// =====================================================
// MAIN FUNCTION - Create Multiple Bots
// =====================================================
bool CreateMultipleBotsAdvanced_StoredProc(int botCount, int startFrom, int gateNumber, int mapNumber, int mapX, int mapY,
    int minLevel, int maxLevel, int selectedClass, int phamViTrain, int moveRange, int timeReturn,
    int tuNhatItem, int tuDongReset, int partyMode, int pvpMode, int postKhiDie, int enabledConfigs)
{
    LogAdd(LOG_BLACK, (char*)"[CreateBots] ===== START =====");
    LogAdd(LOG_BLACK, (char*)"[CreateBots] Count=%d, StartFrom=%d", botCount, startFrom);

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

    // Auto-initialize ODBC if needed
    if (!g_OdbcInitialized)
    {
        LogAdd(LOG_BLUE, (char*)"[CreateBots] Initializing ODBC...");
        extern bool InitializeBotODBC(const char*, const char*, const char*, const char*);
        if (!InitializeBotODBC(".\\SQLEXPRESS", "MuOnline", "", ""))
        {
            LogAdd(LOG_RED, (char*)"[CreateBots] FAILED to initialize ODBC!");
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
        bool useFemaleNames;
    };

    ClassConfig classes[] = {
        {0,  "DW",  9,   12,  16,  -1,  -1, 15, false},
        {16, "DK",  44,  41,  48,  -1,  -1, 30, false},
        {32, "ELF", 24,  52,  26,  27,  28, 20, true},
        {48, "MG",  8,  55,   -1,  -1,  -1, 10, false},
        {64, "DL",  65,  61,  64,  -1,  -1, 10, false},
        {80, "SUM", 214, 215, 217, 218, -1, 10, true},
        {96, "RF",  264, 263, 266, 268, -1, 5,  false}
    };

    int classCount = sizeof(classes) / sizeof(classes[0]);

    // Calculate cumulative percentages for random selection
    int cumulativePercentages[10];
    int total = 0;
    for (int i = 0; i < classCount; i++)
    {
        total += classes[i].percentage;
        cumulativePercentages[i] = total;
    }

    // Open XML file
    FILE* xmlFile = NULL;
    errno_t err = fopen_s(&xmlFile, "IA\\Generated\\IA_Accounts.xml", "w");
    if (err != 0 || !xmlFile)
    {
        LogAdd(LOG_RED, (char*)"[CreateBots] FAILED to open XML file!");
        return false;
    }

    // Write XML header
    fputs("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n\n", xmlFile);
    fputs("<MSGThongBao IndexMesMin=\"3020\" IndexMesMax=\"3030\"/>\n\n", xmlFile);
    fputs("<Config DelayRange=\"40000\" />\n\n", xmlFile);
    fputs("<FakeOnlineData>\n", xmlFile);

    int successCount = 0;
    int failCount = 0;

    for (int i = 0; i < botCount; i++)
    {
        int botNumber = startFrom + i;
        char account[11];
        char charName[11];
        char baseName[50];

        sprintf_s(account, sizeof(account), "Bot%04d", botNumber);

        // =====================================================
        // FIX: Select class correctly for random mode
        // =====================================================
        ClassConfig* selectedClassConfig = NULL;

        if (selectedClass == -1)
        {
            // Random class based on percentage distribution
            int randValue = (GetLargeRand() % 100); // FIX: Use random, not (i % 100)
            
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
            // Specific class selected
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

        // =====================================================
        // FIX: Get name from SQL Database (not .txt file)
        // =====================================================
        const char* gender = selectedClassConfig->useFemaleNames ? "Female" : "Male";
        
        if (!GetRandomBotNameFromSQL(gender, NULL, baseName, sizeof(baseName)))
        {
            // Fallback
            strcpy_s(baseName, sizeof(baseName), "Bot");
        }

        // Create character name (max 10 chars: 8 chars name + 2 digits)
        sprintf_s(charName, sizeof(charName), "%s%02d", baseName, (GetLargeRand() % 90) + 10);
        
        // Extra safety: truncate if too long
        if (strlen(charName) > 10)
            charName[10] = '\0';

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

        // Get hex data from ClassConfigManager (reads from IA/AccountsInventory/ClassConfig.ini)
        const char* invHex = g_ClassConfigManager.GetInventoryHex(selectedClassConfig->classCode, configIndex);
        const char* magicHex = g_ClassConfigManager.GetMagicListHex(selectedClassConfig->classCode, configIndex);

        if (!invHex || !magicHex)
        {
            LogAdd(LOG_RED, (char*)"[CreateBots] Bot %d: Failed to get hex data", i + 1);
            failCount++;
            continue;
        }

        // Create bot in database via stored procedure
        if (CallBotStoredProcedure(account, charName, selectedClassConfig->classCode,
            level, mapNumber, finalMapX, finalMapY,
            0, 0, 0, 0, 0,  // Stats come from DefaultClassType table
            invHex, magicHex))
        {
            successCount++;
            if ((i + 1) % 50 == 0)
            {
                LogAdd(LOG_BLUE, (char*)"[CreateBots] Progress: %d/%d", i + 1, botCount);
            }
        }
        else
        {
            failCount++;
        }
    }

    // Write XML footer
    fputs("</FakeOnlineData>\n", xmlFile);
    fclose(xmlFile);

    LogAdd(LOG_GREEN, (char*)"[CreateBots] ===== COMPLETED =====");
    LogAdd(LOG_GREEN, (char*)"[CreateBots] Success: %d, Failed: %d", successCount, failCount);

    return (successCount > 0);
}
