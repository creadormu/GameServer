// =============================================
// UnicodeODBC.h
// Unicode-enabled ODBC wrapper for MuOnline GameServer
// Supports nvarchar database fields for multilingual characters
// =============================================

#pragma once

#include <string>
#include "UnicodeSupport.h"
// SQL headers are included in stdafx.h

// =============================================
// UNICODE ODBC CONNECTION CLASS
// =============================================

class CUnicodeODBC
{
public:
	CUnicodeODBC() 
		: m_hEnv(SQL_NULL_HENV)
		, m_hDbc(SQL_NULL_HDBC)
		, m_hStmt(SQL_NULL_HSTMT)
		, m_connected(false)
	{
	}

	~CUnicodeODBC()
	{
		Disconnect();
	}

	// Connect to database using Unicode connection string
	bool Connect(const char* server, const char* database, const char* user = nullptr, const char* password = nullptr)
	{
		SQLRETURN ret;

		// Allocate environment handle
		ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv);
		if (!SQL_SUCCEEDED(ret))
			return false;

		// Set ODBC version
		SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);

		// Allocate connection handle
		ret = SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &m_hDbc);
		if (!SQL_SUCCEEDED(ret))
		{
			SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
			return false;
		}

		// Build connection string
		std::wstring connStr;
		if (user != nullptr && password != nullptr)
		{
			// SQL Server authentication
			wchar_t wConnStr[512];
			swprintf_s(wConnStr, 512, L"DRIVER={SQL Server};SERVER=%s;DATABASE=%s;UID=%s;PWD=%s;",
				UTF8ToWide(server).c_str(), UTF8ToWide(database).c_str(),
				UTF8ToWide(user).c_str(), UTF8ToWide(password).c_str());
			connStr = wConnStr;
		}
		else
		{
			// Windows authentication
			wchar_t wConnStr[512];
			swprintf_s(wConnStr, 512, L"DRIVER={SQL Server};SERVER=%s;DATABASE=%s;Trusted_Connection=yes;",
				UTF8ToWide(server).c_str(), UTF8ToWide(database).c_str());
			connStr = wConnStr;
		}

		// Connect using Unicode function
		ret = SQLDriverConnectW(m_hDbc, NULL, (SQLWCHAR*)connStr.c_str(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);

		if (SQL_SUCCEEDED(ret))
		{
			m_connected = true;
			return true;
		}

		// Cleanup on failure
		SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc);
		SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
		m_hDbc = SQL_NULL_HDBC;
		m_hEnv = SQL_NULL_HENV;

		return false;
	}

	// Disconnect from database
	void Disconnect()
	{
		if (m_hStmt != SQL_NULL_HSTMT)
		{
			SQLFreeHandle(SQL_HANDLE_STMT, m_hStmt);
			m_hStmt = SQL_NULL_HSTMT;
		}

		if (m_hDbc != SQL_NULL_HDBC)
		{
			if (m_connected)
			{
				SQLDisconnect(m_hDbc);
				m_connected = false;
			}
			SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc);
			m_hDbc = SQL_NULL_HDBC;
		}

		if (m_hEnv != SQL_NULL_HENV)
		{
			SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
			m_hEnv = SQL_NULL_HENV;
		}
	}

	// Execute Unicode SQL query
	bool ExecuteQuery(const char* query)
	{
		if (!m_connected)
			return false;

		// Free previous statement if exists
		if (m_hStmt != SQL_NULL_HSTMT)
		{
			SQLFreeHandle(SQL_HANDLE_STMT, m_hStmt);
			m_hStmt = SQL_NULL_HSTMT;
		}

		// Allocate statement handle
		SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hStmt);
		if (!SQL_SUCCEEDED(ret))
			return false;

		// Convert query to wide string and execute
		std::wstring wQuery = UTF8ToWide(query);
		ret = SQLExecDirectW(m_hStmt, (SQLWCHAR*)wQuery.c_str(), SQL_NTS);

		return SQL_SUCCEEDED(ret);
	}

	// Fetch next row
	bool Fetch()
	{
		if (m_hStmt == SQL_NULL_HSTMT)
			return false;

		SQLRETURN ret = SQLFetch(m_hStmt);
		return SQL_SUCCEEDED(ret);
	}

	// Get string column value (UTF-8)
	bool GetData(int column, char* buffer, size_t bufferSize)
	{
		if (m_hStmt == SQL_NULL_HSTMT || buffer == nullptr || bufferSize == 0)
			return false;

		SQLWCHAR wBuffer[1024];
		SQLLEN indicator;

		SQLRETURN ret = SQLGetData(m_hStmt, column, SQL_C_WCHAR, wBuffer, sizeof(wBuffer), &indicator);
		if (SQL_SUCCEEDED(ret))
		{
			if (indicator == SQL_NULL_DATA)
			{
				buffer[0] = '\0';
				return true;
			}

			SQLWCHARToChar(wBuffer, buffer, bufferSize);
			return true;
		}

		return false;
	}

	// Get integer column value
	bool GetData(int column, int& value)
	{
		if (m_hStmt == SQL_NULL_HSTMT)
			return false;

		SQLLEN indicator;
		SQLRETURN ret = SQLGetData(m_hStmt, column, SQL_C_LONG, &value, sizeof(value), &indicator);

		if (SQL_SUCCEEDED(ret))
		{
			if (indicator == SQL_NULL_DATA)
			{
				value = 0;
				return true;
			}
			return true;
		}

		return false;
	}

	// Close current statement
	void CloseStatement()
	{
		if (m_hStmt != SQL_NULL_HSTMT)
		{
			SQLFreeHandle(SQL_HANDLE_STMT, m_hStmt);
			m_hStmt = SQL_NULL_HSTMT;
		}
	}

	// Get last error message
	std::string GetLastError()
	{
		SQLWCHAR sqlState[6];
		SQLWCHAR errorMsg[SQL_MAX_MESSAGE_LENGTH];
		SQLINTEGER nativeError;
		SQLSMALLINT msgLen;

		SQLHANDLE handle = (m_hStmt != SQL_NULL_HSTMT) ? m_hStmt :
			(m_hDbc != SQL_NULL_HDBC) ? m_hDbc : m_hEnv;

		SQLSMALLINT handleType = (m_hStmt != SQL_NULL_HSTMT) ? SQL_HANDLE_STMT :
			(m_hDbc != SQL_NULL_HDBC) ? SQL_HANDLE_DBC : SQL_HANDLE_ENV;

		if (SQLGetDiagRecW(handleType, handle, 1, sqlState, &nativeError,
			errorMsg, SQL_MAX_MESSAGE_LENGTH, &msgLen) == SQL_SUCCESS)
		{
			return WideToUTF8((wchar_t*)errorMsg);
		}

		return "Unknown error";
	}

	bool IsConnected() const { return m_connected; }

private:
	SQLHENV m_hEnv;
	SQLHDBC m_hDbc;
	SQLHSTMT m_hStmt;
	bool m_connected;
};
