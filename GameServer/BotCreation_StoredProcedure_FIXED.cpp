// =============================================
// FIXED VERSION - Bot Creation via Stored Procedure
// =============================================
// This version matches your existing GameServer code structure
// =============================================

#include "stdafx.h"
#include "GameServer.h"
#include "Util.h"
#include <sql.h>
#include <sqlext.h>

#pragma comment(lib, "odbc32.lib")
#pragma comment(lib, "odbccp32.lib")

// =============================================
// Find your ODBC connection handle
// Search your code for one of these patterns:
// - SQLHDBC g_something
// - m_hDbc
// - Database connection initialization
// =============================================
// OPTION 1: If you have a global ODBC handle, uncomment and adjust:
// extern SQLHDBC g_YourOdbcHandleName;  // <-- Replace with YOUR actual handle name

// OPTION 2: If you don't have ODBC yet, we'll create a simple one:
SQLHENV g_hOdbcEnv = SQL_NULL_HENV;
SQLHDBC g_hOdbcConn = SQL_NULL_HDBC;

// =====================================================
// Initialize ODBC Connection (call this at server startup)
// =====================================================
bool InitializeBotODBC(const char* server, const char* database, const char* user, const char* password)
{
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
        return false;
    }
    
    // Build connection string
    char connStr[512];
    sprintf_s(connStr, sizeof(connStr),
        "DRIVER={SQL Server};SERVER=%s;DATABASE=%s;UID=%s;PWD=%s;",
        server, database, user, password);
    
    // Connect to database
    ret = SQLDriverConnect(g_hOdbcConn, NULL, (SQLCHAR*)connStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
    
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
    {
        LogAdd(LOG_GREEN, (char*)"[BotODBC] Connected to database successfully");
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
        return false;
    }
}

// =====================================================
// Helper: Call Stored Procedure
// =====================================================
bool CallCreateBotStoredProc(const char* accountID, const char* charName, 
                              int classCode, int level, int mapNumber, 
                              int mapX, int mapY, int str, int dex, int vit, 
                              int ene, int cmd, const char* invHex, const char* magicHex)
{
    SQLHSTMT hStmt = NULL;
    SQLRETURN ret;
    
    // Use YOUR connection handle here
    // OPTION 1: If you have existing ODBC connection, replace g_hOdbcConn with YOUR handle
    // OPTION 2: If using the one we created above, keep g_hOdbcConn
    ret = SQLAllocHandle(SQL_HANDLE_STMT, g_hOdbcConn, &hStmt);
    
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
    {
        LogAdd(LOG_RED, (char*)"[StoredProc] Failed to allocate statement");
        return false;
    }
    
    // Build stored procedure call
    char query[2048];
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
// UTILITY: Batch Creation Using Stored Procedure
// =====================================================
bool CreateBotsViaBatch(int startFrom, int botCount, int minLevel, int maxLevel, 
                         int mapNumber, int mapX, int mapY)
{
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
