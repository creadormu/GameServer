// =============================================
// UnicodeODBC_Example.cpp
// Example code showing how to update ODBC queries to Unicode
// =============================================

#include "stdafx.h"
#include "UnicodeSupport.h"
#include "UnicodeODBC.h"

// SQL headers are included in stdafx.h
// These are EXAMPLE functions - you need to declare g_hOdbcConn in your actual code
extern SQLHDBC g_hOdbcConn; // Your global database connection handle

// =============================================
// EXAMPLE 1: Character Creation with Unicode
// =============================================

bool CreateCharacterUnicode(const char* accountID, const char* characterName, int classType)
{
	// ---- OLD CODE (ASCII only) ----
	/*
	SQLHSTMT hStmt;
	SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

	char query[512];
	sprintf_s(query, "INSERT INTO Character (AccountID, Name, Class) VALUES ('%s', '%s', %d)",
		accountID, characterName, classType);

	SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS);
	*/

	// ---- NEW CODE (Unicode) ----
	SQLHSTMT hStmt;
	SQLAllocHandle(SQL_HANDLE_STMT, g_hOdbcConn, &hStmt); // Assuming global connection

	// Convert parameters to wide string
	std::wstring wAccountID = UTF8ToWide(accountID);
	std::wstring wCharName = UTF8ToWide(characterName);

	// Build Unicode query with N prefix for NVARCHAR literals
	wchar_t wQuery[512];
	swprintf_s(wQuery, 512,
		L"INSERT INTO Character (AccountID, Name, Class) VALUES (N'%s', N'%s', %d)",
		wAccountID.c_str(), wCharName.c_str(), classType);

	// Execute with Unicode function
	SQLRETURN ret = SQLExecDirectW(hStmt, (SQLWCHAR*)wQuery, SQL_NTS);

	bool success = SQL_SUCCEEDED(ret);
	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

	return success;
}

// =============================================
// EXAMPLE 2: Load Character with Unicode Name
// =============================================

bool LoadCharacterUnicode(const char* characterName, int& level, int& strength)
{
	// ---- OLD CODE (ASCII only) ----
	/*
	SQLHSTMT hStmt;
	SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

	char query[256];
	sprintf_s(query, "SELECT cLevel, Strength FROM Character WHERE Name = '%s'", characterName);

	SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS);

	if (SQLFetch(hStmt) == SQL_SUCCESS)
	{
		SQLLEN indicator;
		SQLGetData(hStmt, 1, SQL_C_LONG, &level, sizeof(level), &indicator);
		SQLGetData(hStmt, 2, SQL_C_LONG, &strength, sizeof(strength), &indicator);
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return true;
	}
	*/

	// ---- NEW CODE (Unicode) ----
	SQLHSTMT hStmt;
	SQLAllocHandle(SQL_HANDLE_STMT, g_hOdbcConn, &hStmt);

	// Build Unicode query
	std::wstring wCharName = UTF8ToWide(characterName);
	wchar_t wQuery[256];
	swprintf_s(wQuery, 256,
		L"SELECT cLevel, Strength FROM Character WHERE Name = N'%s'",
		wCharName.c_str());

	// Execute
	SQLRETURN ret = SQLExecDirectW(hStmt, (SQLWCHAR*)wQuery, SQL_NTS);

	if (SQL_SUCCEEDED(ret))
	{
		if (SQLFetch(hStmt) == SQL_SUCCESS)
		{
			SQLLEN indicator;
			SQLGetData(hStmt, 1, SQL_C_LONG, &level, sizeof(level), &indicator);
			SQLGetData(hStmt, 2, SQL_C_LONG, &strength, sizeof(strength), &indicator);
			SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
			return true;
		}
	}

	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	return false;
}

// =============================================
// EXAMPLE 3: Get All Characters for Account
// =============================================

bool GetAccountCharactersUnicode(const char* accountID, char characters[5][11], int& count)
{
	// ---- NEW CODE (Unicode) ----
	SQLHSTMT hStmt;
	SQLAllocHandle(SQL_HANDLE_STMT, g_hOdbcConn, &hStmt);

	// Build Unicode query
	std::wstring wAccountID = UTF8ToWide(accountID);
	wchar_t wQuery[256];
	swprintf_s(wQuery, 256,
		L"SELECT Name FROM Character WHERE AccountID = N'%s' ORDER BY cLevel DESC",
		wAccountID.c_str());

	// Execute
	SQLRETURN ret = SQLExecDirectW(hStmt, (SQLWCHAR*)wQuery, SQL_NTS);

	count = 0;
	if (SQL_SUCCEEDED(ret))
	{
		SQLWCHAR wName[11];
		SQLLEN indicator;

		while (SQLFetch(hStmt) == SQL_SUCCESS && count < 5)
		{
			// Get data as wide string
			SQLGetData(hStmt, 1, SQL_C_WCHAR, wName, sizeof(wName), &indicator);

			// Convert to UTF-8
			SQLWCHARToChar(wName, characters[count], 11);

			count++;
		}
	}

	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	return (count > 0);
}

// =============================================
// EXAMPLE 4: Using Prepared Statements (Best Practice)
// =============================================

bool CreateCharacterPreparedUnicode(const char* accountID, const char* characterName, int classType)
{
	SQLHSTMT hStmt;
	SQLAllocHandle(SQL_HANDLE_STMT, g_hOdbcConn, &hStmt);

	// Prepare Unicode statement
	const wchar_t* wQuery = L"INSERT INTO Character (AccountID, Name, Class) VALUES (?, ?, ?)";
	SQLRETURN ret = SQLPrepareW(hStmt, (SQLWCHAR*)wQuery, SQL_NTS);

	if (!SQL_SUCCEEDED(ret))
	{
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return false;
	}

	// Convert parameters to wide strings
	SQLWCHAR wAccountID[11];
	SQLWCHAR wCharName[11];
	CharToSQLWCHAR(accountID, wAccountID, 11);
	CharToSQLWCHAR(characterName, wCharName, 11);

	// Bind parameters
	SQLLEN cbAccountID = SQL_NTS;
	SQLLEN cbCharName = SQL_NTS;
	SQLLEN cbClassType = 0;

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 10, 0, wAccountID, 0, &cbAccountID);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 10, 0, wCharName, 0, &cbCharName);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &classType, 0, &cbClassType);

	// Execute
	ret = SQLExecute(hStmt);
	bool success = SQL_SUCCEEDED(ret);

	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	return success;
}

// =============================================
// EXAMPLE 5: Update with Unicode (Character Rename)
// =============================================

bool RenameCharacterUnicode(const char* oldName, const char* newName)
{
	SQLHSTMT hStmt;
	SQLAllocHandle(SQL_HANDLE_STMT, g_hOdbcConn, &hStmt);

	// Build Unicode query
	std::wstring wOldName = UTF8ToWide(oldName);
	std::wstring wNewName = UTF8ToWide(newName);

	wchar_t wQuery[512];
	swprintf_s(wQuery, 512,
		L"UPDATE Character SET Name = N'%s' WHERE Name = N'%s'",
		wNewName.c_str(), wOldName.c_str());

	SQLRETURN ret = SQLExecDirectW(hStmt, (SQLWCHAR*)wQuery, SQL_NTS);
	bool success = SQL_SUCCEEDED(ret);

	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	return success;
}

// =============================================
// EXAMPLE 6: Using CUnicodeODBC Wrapper Class
// =============================================

bool CreateCharacterWithWrapper(const char* accountID, const char* characterName, int classType)
{
	// Using the wrapper class - much simpler!
	CUnicodeODBC db;

	if (!db.Connect("localhost", "MuOnline"))
		return false;

	// Build query (automatic conversion)
	char query[512];
	sprintf_s(query,
		"INSERT INTO Character (AccountID, Name, Class) VALUES (N'%s', N'%s', %d)",
		accountID, characterName, classType);

	bool result = db.ExecuteQuery(query);

	db.Disconnect();
	return result;
}

bool LoadCharacterWithWrapper(const char* characterName, int& level)
{
	CUnicodeODBC db;

	if (!db.Connect("localhost", "MuOnline"))
		return false;

	char query[256];
	sprintf_s(query, "SELECT cLevel FROM Character WHERE Name = N'%s'", characterName);

	if (!db.ExecuteQuery(query))
	{
		db.Disconnect();
		return false;
	}

	bool found = false;
	if (db.Fetch())
	{
		db.GetData(1, level);
		found = true;
	}

	db.Disconnect();
	return found;
}

// =============================================
// EXAMPLE 7: Bulk Load Characters
// =============================================

struct CharacterInfo
{
	char name[11];
	int level;
	int classType;
};

int LoadAllCharactersUnicode(const char* accountID, CharacterInfo* characters, int maxCount)
{
	CUnicodeODBC db;

	if (!db.Connect("localhost", "MuOnline"))
		return 0;

	char query[256];
	sprintf_s(query,
		"SELECT Name, cLevel, Class FROM Character WHERE AccountID = N'%s' ORDER BY cLevel DESC",
		accountID);

	if (!db.ExecuteQuery(query))
	{
		db.Disconnect();
		return 0;
	}

	int count = 0;
	while (db.Fetch() && count < maxCount)
	{
		db.GetData(1, characters[count].name, sizeof(characters[count].name));
		db.GetData(2, characters[count].level);
		db.GetData(3, characters[count].classType);
		count++;
	}

	db.Disconnect();
	return count;
}

// =============================================
// EXAMPLE 8: Calling Stored Procedure with Unicode
// =============================================

bool CallStoredProcedureUnicode(const char* accountID, const char* characterName, int classType)
{
	SQLHSTMT hStmt;
	SQLAllocHandle(SQL_HANDLE_STMT, g_hOdbcConn, &hStmt);

	// Prepare stored procedure call
	const wchar_t* wQuery = L"{CALL WZ_CreateCharacter (?, ?, ?)}";
	SQLRETURN ret = SQLPrepareW(hStmt, (SQLWCHAR*)wQuery, SQL_NTS);

	if (!SQL_SUCCEEDED(ret))
	{
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return false;
	}

	// Convert and bind parameters
	SQLWCHAR wAccountID[11];
	SQLWCHAR wCharName[11];
	CharToSQLWCHAR(accountID, wAccountID, 11);
	CharToSQLWCHAR(characterName, wCharName, 11);

	SQLLEN cbAccountID = SQL_NTS;
	SQLLEN cbCharName = SQL_NTS;
	SQLLEN cbClassType = 0;

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 10, 0, wAccountID, 0, &cbAccountID);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 10, 0, wCharName, 0, &cbCharName);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &classType, 0, &cbClassType);

	// Execute
	ret = SQLExecute(hStmt);
	bool success = SQL_SUCCEEDED(ret);

	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	return success;
}

// =============================================
// COMMON PATTERNS SUMMARY
// =============================================

/*
1. ALWAYS use SQLExecDirectW instead of SQLExecDirect
2. ALWAYS use SQL_C_WCHAR for string parameters
3. ALWAYS prefix NVARCHAR literals with N'...'
4. ALWAYS convert char* to SQLWCHAR* for binding
5. ALWAYS convert SQLWCHAR* results back to char* (UTF-8)

KEY FUNCTIONS:
- SQLDriverConnectW() - Connect to database
- SQLExecDirectW() - Execute immediate query
- SQLPrepareW() - Prepare statement
- SQL_C_WCHAR - Data type for Unicode strings
- SQL_WVARCHAR - SQL type for NVARCHAR columns

HELPER FUNCTIONS (from UnicodeSupport.h):
- UTF8ToWide() - Convert UTF-8 char* to wstring
- WideToUTF8() - Convert wstring to UTF-8 char*
- CharToSQLWCHAR() - Convert char* to SQLWCHAR*
- SQLWCHARToChar() - Convert SQLWCHAR* to char*
*/
