// NameManager.cpp - Multi-Language Name Loading System
#include "stdafx.h"
#include "NameManager.h"
#include "Util.h"
#include <fstream>
#include <windows.h>

CNameManager g_NameManager;

CNameManager::CNameManager()
{
    sprintf_s(m_BasePath, sizeof(m_BasePath), "IA\\Names\\");
    memset(m_CurrentLanguage, 0, sizeof(m_CurrentLanguage));
    memset(m_AvailableLanguages, 0, sizeof(m_AvailableLanguages));
    m_MaleNameCount = 0;
    m_FemaleNameCount = 0;
    m_AvailableLanguageCount = 0;
}

CNameManager::~CNameManager()
{
    ClearNames();
}

void CNameManager::Initialize()
{
    LogAdd(LOG_BLACK, "[NameManager] Initializing...");

    // Scan for available language folders
    if (!ScanAvailableLanguages())
    {
        LogAdd(LOG_BLUE, "[NameManager] No language folders found, using default names");
        LoadDefaultNames();
        return;
    }

    // Try to load Spanish by default (or first available language)
    bool loaded = false;
    if (m_AvailableLanguageCount > 0)
    {
        // Try Spanish first
        for (int i = 0; i < m_AvailableLanguageCount; i++)
        {
            if (_stricmp(m_AvailableLanguages[i], "Spanish") == 0)
            {
                loaded = LoadLanguage("Spanish");
                break;
            }
        }

        // If Spanish not found, load first available
        if (!loaded && m_AvailableLanguageCount > 0)
        {
            loaded = LoadLanguage(m_AvailableLanguages[0]);
        }
    }

    if (!loaded)
    {
        LogAdd(LOG_BLUE, "[NameManager] Could not load language files, using defaults");
        LoadDefaultNames();
    }
}

bool CNameManager::ScanAvailableLanguages()
{
    m_AvailableLanguageCount = 0;

    WIN32_FIND_DATA findData;
    char searchPath[260];
    sprintf_s(searchPath, sizeof(searchPath), "%s*", m_BasePath);

    HANDLE hFind = FindFirstFile(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    do
    {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // Skip . and ..
            if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0)
            {
                if (m_AvailableLanguageCount < MAX_LANGUAGES)
                {
                    sprintf_s(m_AvailableLanguages[m_AvailableLanguageCount],
                        sizeof(m_AvailableLanguages[m_AvailableLanguageCount]),
                        "%s", findData.cFileName);
                    m_AvailableLanguageCount++;
                }
            }
        }
    } while (FindNextFile(hFind, &findData) != 0);

    FindClose(hFind);

    if (m_AvailableLanguageCount > 0)
    {
        LogAdd(LOG_GREEN, "[NameManager] Found %d language(s):", m_AvailableLanguageCount);
        for (int i = 0; i < m_AvailableLanguageCount; i++)
        {
            LogAdd(LOG_BLACK, "  - %s", m_AvailableLanguages[i]);
        }
    }

    return (m_AvailableLanguageCount > 0);
}

bool CNameManager::LoadLanguage(const char* languageName)
{
    if (!languageName || languageName[0] == 0)
    {
        return false;
    }

    char languagePath[260];
    sprintf_s(languagePath, sizeof(languagePath), "%s%s\\", m_BasePath, languageName);

    return LoadLanguageFromPath(languagePath);
}

bool CNameManager::LoadLanguageFromPath(const char* folderPath)
{
    ClearNames();

    char malePath[260];
    char femalePath[260];
    sprintf_s(malePath, sizeof(malePath), "%sMaleNames.txt", folderPath);
    sprintf_s(femalePath, sizeof(femalePath), "%sFemaleNames.txt", folderPath);

    bool maleLoaded = LoadNamesFromFile(malePath, true);
    bool femaleLoaded = LoadNamesFromFile(femalePath, false);

    if (maleLoaded || femaleLoaded)
    {
        // Extract language name from path
        const char* lastSlash = strrchr(folderPath, '\\');
        if (lastSlash)
        {
            const char* prevSlash = lastSlash - 1;
            while (prevSlash > folderPath && *prevSlash != '\\') prevSlash--;
            if (*prevSlash == '\\') prevSlash++;

            size_t len = lastSlash - prevSlash;
            if (len > 0 && len < sizeof(m_CurrentLanguage))
            {
                memcpy(m_CurrentLanguage, prevSlash, len);
                m_CurrentLanguage[len] = 0;
            }
        }

        LogAdd(LOG_GREEN, "[NameManager] Loaded language: %s (%d male, %d female names)",
            m_CurrentLanguage, m_MaleNameCount, m_FemaleNameCount);
        return true;
    }

    return false;
}

bool CNameManager::LoadNamesFromFile(const char* filePath, bool isMale)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        LogAdd(LOG_RED, "[NameManager] Cannot open file: %s", filePath);
        return false;
    }

    std::string line;
    int count = 0;

    while (std::getline(file, line))
    {
        // Remove whitespace and newlines
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        if (!line.empty())
        {
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
        }

        // Skip empty lines and comments
        if (line.empty() || line[0] == ';' || line[0] == '#')
            continue;

        // Validate name length
        if (line.length() >= MAX_NAME_LENGTH)
        {
            LogAdd(LOG_RED, "[NameManager] Name too long (max %d chars): %s",
                MAX_NAME_LENGTH - 1, line.c_str());
            continue;
        }

        // Add name
        if (isMale)
        {
            m_MaleNames.push_back(line);
        }
        else
        {
            m_FemaleNames.push_back(line);
        }
        count++;
    }

    file.close();

    if (isMale)
    {
        m_MaleNameCount = m_MaleNames.size();
    }
    else
    {
        m_FemaleNameCount = m_FemaleNames.size();
    }

    return (count > 0);
}

void CNameManager::LoadDefaultNames()
{
    // Spanish names as fallback
    sprintf_s(m_CurrentLanguage, sizeof(m_CurrentLanguage), "Default");

    const char* defaultMaleNames[] = {
        "Carlos", "Diego", "Miguel", "Juan", "Pedro", "Luis", "Jorge", "Fernando", "Ricardo", "Roberto",
        "Sergio", "Andres", "Javier", "Marco", "Oscar", "Daniel", "Gabriel", "Rafael", "Adrian", "Mario",
        "Eduardo", "Ernesto", "Pablo", "Raul", "Alberto", "Victor", "Manuel", "Felipe", "Emilio", "Hugo"
    };

    const char* defaultFemaleNames[] = {
        "Maria", "Ana", "Sofia", "Isabella", "Valentina", "Camila", "Victoria", "Lucia", "Elena", "Paula",
        "Carmen", "Laura", "Diana", "Andrea", "Natalia", "Carolina", "Gabriela", "Daniela", "Alejandra", "Fernanda"
    };

    m_MaleNames.clear();
    m_FemaleNames.clear();

    for (int i = 0; i < 30; i++)
    {
        m_MaleNames.push_back(defaultMaleNames[i]);
    }

    for (int i = 0; i < 20; i++)
    {
        m_FemaleNames.push_back(defaultFemaleNames[i]);
    }

    m_MaleNameCount = m_MaleNames.size();
    m_FemaleNameCount = m_FemaleNames.size();

    LogAdd(LOG_BLUE, "[NameManager] Using default names (%d male, %d female)",
        m_MaleNameCount, m_FemaleNameCount);
}

const char* CNameManager::GetRandomMaleName()
{
    if (m_MaleNameCount == 0)
    {
        LoadDefaultNames();
    }

    if (m_MaleNameCount > 0)
    {
        int index = GetLargeRand() % m_MaleNameCount;
        return m_MaleNames[index].c_str();
    }

    return "Bot";
}

const char* CNameManager::GetRandomFemaleName()
{
    if (m_FemaleNameCount == 0)
    {
        LoadDefaultNames();
    }

    if (m_FemaleNameCount > 0)
    {
        int index = GetLargeRand() % m_FemaleNameCount;
        return m_FemaleNames[index].c_str();
    }

    return "Bot";
}

const char* CNameManager::GetLanguageName(int index) const
{
    if (index >= 0 && index < m_AvailableLanguageCount)
    {
        return m_AvailableLanguages[index];
    }
    return "";
}

void CNameManager::ClearNames()
{
    m_MaleNames.clear();
    m_FemaleNames.clear();
    m_MaleNameCount = 0;
    m_FemaleNameCount = 0;
}
