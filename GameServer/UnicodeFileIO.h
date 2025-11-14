// =============================================
// UnicodeFileIO.h
// UTF-8 File Reading and Writing Utilities
// For configuration files, text files with Unicode content
// =============================================

#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <windows.h>
#include "UnicodeSupport.h"

// =============================================
// UTF-8 FILE READING
// =============================================

// Read entire UTF-8 file into string vector (one line per element)
inline bool ReadUTF8File(const char* filename, std::vector<std::string>& lines)
{
	lines.clear();

	// Open file in binary mode to preserve UTF-8 BOM
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open())
		return false;

	// Check for UTF-8 BOM (EF BB BF)
	char bom[3];
	file.read(bom, 3);
	if (!(bom[0] == (char)0xEF && bom[1] == (char)0xBB && bom[2] == (char)0xBF))
	{
		// No BOM, rewind
		file.seekg(0);
	}

	std::string line;
	while (std::getline(file, line))
	{
		// Remove carriage return if present (Windows line ending)
		if (!line.empty() && line.back() == '\r')
			line.pop_back();

		lines.push_back(line);
	}

	file.close();
	return true;
}

// Read entire UTF-8 file into a single string
inline bool ReadUTF8FileToString(const char* filename, std::string& content)
{
	content.clear();

	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open())
		return false;

	// Check for UTF-8 BOM
	char bom[3];
	file.read(bom, 3);
	if (!(bom[0] == (char)0xEF && bom[1] == (char)0xBB && bom[2] == (char)0xBF))
	{
		file.seekg(0);
	}

	// Read entire file
	file.seekg(0, std::ios::end);
	size_t size = file.tellg();
	file.seekg(0, std::ios::beg);

	content.resize(size);
	file.read(&content[0], size);

	file.close();
	return true;
}

// =============================================
// UTF-8 FILE WRITING
// =============================================

// Write string vector to UTF-8 file (with BOM)
inline bool WriteUTF8File(const char* filename, const std::vector<std::string>& lines, bool writeBOM = true)
{
	std::ofstream file(filename, std::ios::binary);
	if (!file.is_open())
		return false;

	// Write UTF-8 BOM
	if (writeBOM)
	{
		const char bom[3] = { (char)0xEF, (char)0xBB, (char)0xBF };
		file.write(bom, 3);
	}

	// Write lines
	for (size_t i = 0; i < lines.size(); i++)
	{
		file.write(lines[i].c_str(), lines[i].length());
		if (i < lines.size() - 1)
			file.write("\r\n", 2); // Windows line ending
	}

	file.close();
	return true;
}

// Write string to UTF-8 file (with BOM)
inline bool WriteUTF8FileFromString(const char* filename, const std::string& content, bool writeBOM = true)
{
	std::ofstream file(filename, std::ios::binary);
	if (!file.is_open())
		return false;

	// Write UTF-8 BOM
	if (writeBOM)
	{
		const char bom[3] = { (char)0xEF, (char)0xBB, (char)0xBF };
		file.write(bom, 3);
	}

	file.write(content.c_str(), content.length());
	file.close();
	return true;
}

// Append string to UTF-8 file (without BOM)
inline bool AppendUTF8File(const char* filename, const std::string& content)
{
	std::ofstream file(filename, std::ios::binary | std::ios::app);
	if (!file.is_open())
		return false;

	file.write(content.c_str(), content.length());
	file.close();
	return true;
}

// =============================================
// INI FILE UTILITIES (UTF-8)
// =============================================

// Read INI file value (UTF-8 compatible)
inline std::string ReadINIValueUTF8(const char* filename, const char* section, const char* key, const char* defaultValue = "")
{
	wchar_t buffer[1024];
	
	std::wstring wFilename = UTF8ToWide(filename);
	std::wstring wSection = UTF8ToWide(section);
	std::wstring wKey = UTF8ToWide(key);
	std::wstring wDefault = UTF8ToWide(defaultValue);

	GetPrivateProfileStringW(
		wSection.c_str(),
		wKey.c_str(),
		wDefault.c_str(),
		buffer,
		1024,
		wFilename.c_str()
	);

	return WideToUTF8(buffer);
}

// Write INI file value (UTF-8 compatible)
inline bool WriteINIValueUTF8(const char* filename, const char* section, const char* key, const char* value)
{
	std::wstring wFilename = UTF8ToWide(filename);
	std::wstring wSection = UTF8ToWide(section);
	std::wstring wKey = UTF8ToWide(key);
	std::wstring wValue = UTF8ToWide(value);

	return WritePrivateProfileStringW(
		wSection.c_str(),
		wKey.c_str(),
		wValue.c_str(),
		wFilename.c_str()
	) != 0;
}

// =============================================
// TEXT FILE PARSING UTILITIES
// =============================================

// Parse CSV line (UTF-8 aware)
inline std::vector<std::string> ParseCSVLine(const std::string& line, char delimiter = ',')
{
	std::vector<std::string> result;
	std::string current;
	bool inQuotes = false;

	for (size_t i = 0; i < line.length(); i++)
	{
		char c = line[i];

		if (c == '"')
		{
			inQuotes = !inQuotes;
		}
		else if (c == delimiter && !inQuotes)
		{
			result.push_back(current);
			current.clear();
		}
		else
		{
			current += c;
		}
	}

	if (!current.empty())
		result.push_back(current);

	return result;
}

// Trim whitespace from UTF-8 string
inline std::string TrimUTF8(const std::string& str)
{
	if (str.empty())
		return str;

	size_t start = 0;
	size_t end = str.length() - 1;

	// Trim from start
	while (start < str.length() && (str[start] == ' ' || str[start] == '\t' || str[start] == '\r' || str[start] == '\n'))
		start++;

	// Trim from end
	while (end > start && (str[end] == ' ' || str[end] == '\t' || str[end] == '\r' || str[end] == '\n'))
		end--;

	return str.substr(start, end - start + 1);
}

// Check if file is UTF-8 encoded
inline bool IsFileUTF8(const char* filename)
{
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open())
		return false;

	// Check for UTF-8 BOM
	char bom[3];
	file.read(bom, 3);
	file.close();

	return (bom[0] == (char)0xEF && bom[1] == (char)0xBB && bom[2] == (char)0xBF);
}

// Convert file from ANSI to UTF-8
inline bool ConvertFileToUTF8(const char* filename)
{
	// Read as ANSI
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open())
		return false;

	std::string ansiContent;
	file.seekg(0, std::ios::end);
	size_t size = file.tellg();
	file.seekg(0, std::ios::beg);
	ansiContent.resize(size);
	file.read(&ansiContent[0], size);
	file.close();

	// Convert to UTF-8
	std::string utf8Content = ANSIToUTF8(ansiContent.c_str());

	// Write as UTF-8 with BOM
	return WriteUTF8FileFromString(filename, utf8Content, true);
}
