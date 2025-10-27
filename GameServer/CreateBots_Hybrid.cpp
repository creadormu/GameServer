// =============================================
// HYBRID APPROACH: Use Stored Procedure + Keep XML Generation
// =============================================
// This integrates stored procedure into your existing function
// Keeps all your current logic, just replaces SQL generation
// =============================================

#include "stdafx.h"
#include "GameServer.h"
#include <windows.h>
#include <sql.h>
#include <sqlext.h>

extern SQLHDBC g_hOdbcConnection;  // Adjust to your actual ODBC handle name
extern void LogAdd(int color, const char* format, ...);

// Helper function to call stored procedure
bool CallCreateBotStoredProc(const char* accountID, const char* charName, 
                              int classCode, int level, int mapNumber, 
                              int mapX, int mapY, int str, int dex, int vit, 
                              int ene, int cmd, const char* invHex, const char* magicHex)
{
    SQLHSTMT hStmt = NULL;
    SQLRETURN ret;
    
    ret = SQLAllocHandle(SQL_HANDLE_STMT, g_hOdbcConnection, &hStmt);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
    {
        LogAdd(LOG_RED, "[StoredProc] Failed to allocate statement");
        return false;
    }
    
    // Build the procedure call
    char query[2048];
    sprintf_s(query, sizeof(query),
        "EXEC WZ_CreateBotDirect '%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', '%s'",
        accountID, charName, classCode, level, mapNumber, mapX, mapY,
        str, dex, vit, ene, cmd, invHex, magicHex);
    
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
            LogAdd(LOG_RED, "[StoredProc] Error: %s", errorMsg);
        }
    }
    
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return success;
}

// =====================================================
// MODIFIED VERSION OF YOUR FUNCTION
// Uses stored procedure instead of SQL file
// =====================================================
bool CreateMultipleBotsAdvanced(int botCount, int startFrom, int gateNumber, int mapNumber, int mapX, int mapY,
    int minLevel, int maxLevel, int selectedClass, int phamViTrain, int moveRange, int timeReturn,
    int tuNhatItem, int tuDongReset, int partyMode, int pvpMode, int postKhiDie, int enabledConfigs)
{
    LogAdd(LOG_BLACK, "[CreateBots] ===== START (STORED PROCEDURE METHOD) =====");
    LogAdd(LOG_BLACK, "[CreateBots] Count=%d, StartFrom=%d, EnabledConfigs=%d", botCount, startFrom, enabledConfigs);

    // SAFETY CHECKS
    if (botCount > 1000 || botCount < 1)
    {
        LogAdd(LOG_RED, "[CreateBots] ERROR: Invalid bot count");
        return false;
    }

    if (startFrom < 1)
    {
        LogAdd(LOG_RED, "[CreateBots] ERROR: Invalid start number");
        return false;
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

    LogAdd(LOG_BLACK, "[CreateBots] Opening XML file...");

    // =====================================================
    // PART 1: GENERATE XML FILE (Keep this - needed for bot AI)
    // =====================================================
    FILE* xmlFile = NULL;
    errno_t err = fopen_s(&xmlFile, "IA\\Generated\\IA_Accounts.xml", "w");
    if (err != 0 || !xmlFile)
    {
        LogAdd(LOG_RED, "[CreateBots] FAILED to open XML file! Error: %d", err);
        return false;
    }
    LogAdd(LOG_GREEN, "[CreateBots] XML file opened successfully");

    // Write XML header
    fputs("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n\n", xmlFile);
    fputs("<MSGThongBao IndexMesMin=\"3020\" IndexMesMax=\"3030\"/>\n\n", xmlFile);
    fputs("<Config DelayRange=\"40000\" />\n\n", xmlFile);
    fputs("<FakeOnlineData>\n", xmlFile);

    LogAdd(LOG_GREEN, "[CreateBots] Starting bot generation...");

    // =====================================================
    // PART 2: CREATE BOTS - CALL STORED PROCEDURE INSTEAD OF SQL FILE
    // =====================================================
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
            LogAdd(LOG_RED, "[CreateBots] Class %s NOT configured in Config %d!",
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

        // =====================================================
        // WRITE TO XML (Keep this - needed for bot AI)
        // =====================================================
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

        // =====================================================
        // CREATE IN DATABASE VIA STORED PROCEDURE (NEW!)
        // No more SQL file generation!
        // =====================================================
        const char* invHex = g_ClassConfigManager.GetInventoryHex(selectedClassConfig->classCode, configIndex);
        const char* magicHex = g_ClassConfigManager.GetMagicListHex(selectedClassConfig->classCode, configIndex);

        if (!invHex || !magicHex)
        {
            LogAdd(LOG_RED, "[CreateBots] Bot %d: Failed to get hex data", i + 1);
            failCount++;
            continue;
        }

        // Call stored procedure to create bot in database
        if (CallCreateBotStoredProc(account, charName, selectedClassConfig->classCode,
                                     level, mapNumber, finalMapX, finalMapY,
                                     selectedClassConfig->str, selectedClassConfig->dex, 
                                     selectedClassConfig->vit, selectedClassConfig->ene, 
                                     selectedClassConfig->cmd, invHex, magicHex))
        {
            successCount++;
            if ((i + 1) % 10 == 0)
            {
                LogAdd(LOG_BLUE, "[CreateBots] Progress: %d/%d (Success: %d, Failed: %d)", 
                       i + 1, botCount, successCount, failCount);
            }
        }
        else
        {
            failCount++;
            LogAdd(LOG_RED, "[CreateBots] Bot %d (%s) creation failed", i + 1, account);
        }
    }

    // Write XML footer
    fputs("</FakeOnlineData>\n", xmlFile);
    fclose(xmlFile);

    LogAdd(LOG_GREEN, "[CreateBots] ===== COMPLETED =====");
    LogAdd(LOG_GREEN, "[CreateBots] Success: %d, Failed: %d", successCount, failCount);
    LogAdd(LOG_GREEN, "[CreateBots] XML file: IA\\Generated\\IA_Accounts.xml");

    return (successCount > 0);
}
