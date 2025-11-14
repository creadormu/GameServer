// =============================================
// UnicodeSupport.h
// Unicode string handling utilities for MuOnline GameServer
// Supports Portuguese, Chinese, Japanese, Korean, and other languages
// =============================================

#pragma once

#include <string>
// SQL headers are included in stdafx.h

// =============================================
// UNICODE STRING CONVERSION UTILITIES
// =============================================

// Convert UTF-8 to UTF-16 (Wide String)
inline std::wstring UTF8ToWide(const char* utf8Str)
{
	if (utf8Str == nullptr || utf8Str[0] == '\0')
		return L"";

	int wideLen = MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, nullptr, 0);
	if (wideLen == 0)
		return L"";

	std::wstring wideStr(wideLen - 1, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, &wideStr[0], wideLen);
	return wideStr;
}

// Convert UTF-16 (Wide String) to UTF-8
inline std::string WideToUTF8(const wchar_t* wideStr)
{
	if (wideStr == nullptr || wideStr[0] == L'\0')
		return "";

	int utf8Len = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, nullptr, 0, nullptr, nullptr);
	if (utf8Len == 0)
		return "";

	std::string utf8Str(utf8Len - 1, '\0');
	WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, &utf8Str[0], utf8Len, nullptr, nullptr);
	return utf8Str;
}

// Convert ANSI (system codepage) to UTF-8
inline std::string ANSIToUTF8(const char* ansiStr)
{
	if (ansiStr == nullptr || ansiStr[0] == '\0')
		return "";

	// First convert ANSI to Wide
	int wideLen = MultiByteToWideChar(CP_ACP, 0, ansiStr, -1, nullptr, 0);
	if (wideLen == 0)
		return "";

	std::wstring wideStr(wideLen - 1, L'\0');
	MultiByteToWideChar(CP_ACP, 0, ansiStr, -1, &wideStr[0], wideLen);

	// Then convert Wide to UTF-8
	return WideToUTF8(wideStr.c_str());
}

// Convert UTF-8 to ANSI (system codepage) - may lose characters!
inline std::string UTF8ToANSI(const char* utf8Str)
{
	if (utf8Str == nullptr || utf8Str[0] == '\0')
		return "";

	// First convert UTF-8 to Wide
	std::wstring wideStr = UTF8ToWide(utf8Str);
	if (wideStr.empty())
		return "";

	// Then convert Wide to ANSI
	int ansiLen = WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (ansiLen == 0)
		return "";

	std::string ansiStr(ansiLen - 1, '\0');
	WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), -1, &ansiStr[0], ansiLen, nullptr, nullptr);
	return ansiStr;
}

// Safe copy UTF-8 string to char buffer
inline void SafeCopyUTF8(char* dest, size_t destSize, const char* src)
{
	if (dest == nullptr || destSize == 0)
		return;

	if (src == nullptr || src[0] == '\0')
	{
		dest[0] = '\0';
		return;
	}

	// Copy up to destSize-1 bytes
	size_t i = 0;
	while (i < destSize - 1 && src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
}

// Get UTF-8 string length in characters (not bytes)
inline size_t UTF8Length(const char* utf8Str)
{
	if (utf8Str == nullptr)
		return 0;

	size_t len = 0;
	while (*utf8Str)
	{
		if ((*utf8Str & 0xC0) != 0x80) // Not a continuation byte
			len++;
		utf8Str++;
	}
	return len;
}

// Truncate UTF-8 string to maximum byte length (respects character boundaries)
inline void TruncateUTF8(char* utf8Str, size_t maxBytes)
{
	if (utf8Str == nullptr || maxBytes == 0)
		return;

	size_t len = strlen(utf8Str);
	if (len < maxBytes)
		return;

	// Walk back to find valid UTF-8 boundary
	size_t pos = maxBytes - 1;
	while (pos > 0 && (utf8Str[pos] & 0xC0) == 0x80)
	{
		pos--;
	}

	utf8Str[pos] = '\0';
}

// Validate UTF-8 string
inline bool IsValidUTF8(const char* utf8Str)
{
	if (utf8Str == nullptr)
		return false;

	while (*utf8Str)
	{
		unsigned char c = *utf8Str;

		// Single byte (ASCII)
		if ((c & 0x80) == 0)
		{
			utf8Str++;
			continue;
		}

		// Multi-byte sequence
		int extraBytes = 0;
		if ((c & 0xE0) == 0xC0) extraBytes = 1;
		else if ((c & 0xF0) == 0xE0) extraBytes = 2;
		else if ((c & 0xF8) == 0xF0) extraBytes = 3;
		else return false; // Invalid UTF-8 start byte

		// Check continuation bytes
		for (int i = 0; i < extraBytes; i++)
		{
			utf8Str++;
			if ((*utf8Str & 0xC0) != 0x80)
				return false;
		}

		utf8Str++;
	}

	return true;
}

// =============================================
// ODBC UNICODE HELPERS
// =============================================

// Convert char* to SQLWCHAR* for Unicode ODBC operations
inline SQLWCHAR* CharToSQLWCHAR(const char* str, SQLWCHAR* buffer, size_t bufferSize)
{
	if (str == nullptr || buffer == nullptr || bufferSize == 0)
		return nullptr;

	std::wstring wstr = UTF8ToWide(str);
	wcsncpy_s((wchar_t*)buffer, bufferSize, wstr.c_str(), _TRUNCATE);
	return buffer;
}

// Convert SQLWCHAR* to char* buffer
inline void SQLWCHARToChar(const SQLWCHAR* sqlwStr, char* buffer, size_t bufferSize)
{
	if (sqlwStr == nullptr || buffer == nullptr || bufferSize == 0)
		return;

	std::string utf8Str = WideToUTF8((const wchar_t*)sqlwStr);
	strncpy_s(buffer, bufferSize, utf8Str.c_str(), _TRUNCATE);
}
