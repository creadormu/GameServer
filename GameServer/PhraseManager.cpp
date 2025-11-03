// PhraseManager.cpp - Multi-Language Bot Phrases and Answers System
#include "stdafx.h"
#include "PhraseManager.h"
#include "Util.h"
#include "LanguageConfig.h"
#include <algorithm>
#include <windows.h>

CPhraseManager g_PhraseManager;

CPhraseManager::CPhraseManager()
{
    sprintf_s(m_PhrasesBasePath, sizeof(m_PhrasesBasePath), "IA\\Phrases\\");
    sprintf_s(m_AnswersBasePath, sizeof(m_AnswersBasePath), "IA\\Answers\\");
    memset(m_CurrentLanguage, 0, sizeof(m_CurrentLanguage));
    memset(m_CurrentBotPhrasesPath, 0, sizeof(m_CurrentBotPhrasesPath));
    memset(m_CurrentAnsweringPath, 0, sizeof(m_CurrentAnsweringPath));
    m_PhrasesLoaded = false;
    m_AnswersLoaded = false;
}

CPhraseManager::~CPhraseManager()
{
}

void CPhraseManager::Initialize()
{
    LogAdd(LOG_BLACK, "[PhraseManager] Initializing...");

    if (!g_LanguageConfig.IsLoaded())
    {
        g_LanguageConfig.Load("IA\\languages.ini");
    }

    // Try to load Spanish by default
    if (!LoadLanguage("Spanish"))
    {
        LogAdd(LOG_BLUE, "[PhraseManager] Could not load Spanish phrases, using default paths");
        LoadDefaultPaths();
    }
}

bool CPhraseManager::LoadLanguage(const char* languageName)
{
    if (!languageName || languageName[0] == 0)
    {
        return false;
    }

    const LanguageConfigEntry* entry = g_LanguageConfig.FindLanguage(languageName);
    if (entry)
    {
        const char* phrasesPath = entry->HasPhrases() ? entry->phrasesDir.c_str() : nullptr;
        const char* answersPath = entry->HasAnswers() ? entry->answersDir.c_str() : nullptr;

        if (LoadLanguageFromPath(phrasesPath, answersPath, languageName))
        {
            return true;
        }

        LogAdd(LOG_RED, "[PhraseManager] Failed to load language '%s' from config", languageName);
    }

    char phrasesFolder[260];
    char answersFolder[260];

    sprintf_s(phrasesFolder, sizeof(phrasesFolder), "%s%s\\", m_PhrasesBasePath, languageName);
    sprintf_s(answersFolder, sizeof(answersFolder), "%s%s\\", m_AnswersBasePath, languageName);

    return LoadLanguageFromPath(phrasesFolder, answersFolder, languageName);
}

bool CPhraseManager::LoadLanguageFromPath(const char* phrasesFolder, const char* answersFolder, const char* languageNameOverride)
{
    m_PhrasesLoaded = false;
    m_AnswersLoaded = false;

    auto NormalizeFolder = [](const char* folder) -> std::string
    {
        if (folder == nullptr)
        {
            return std::string();
        }

        std::string normalized = folder;
        std::replace(normalized.begin(), normalized.end(), '/', '\\');
        if (!normalized.empty() && normalized.back() != '\\')
        {
            normalized.push_back('\\');
        }
        return normalized;
    };

    std::string normalizedPhrases = NormalizeFolder(phrasesFolder);
    std::string normalizedAnswers = NormalizeFolder(answersFolder);

    if (!normalizedPhrases.empty())
    {
        m_PhrasesLoaded = LoadPhrasesPath(normalizedPhrases.c_str());
    }

    if (!normalizedAnswers.empty())
    {
        m_AnswersLoaded = LoadAnswersPath(normalizedAnswers.c_str());
    }

    if (m_PhrasesLoaded || m_AnswersLoaded)
    {
        if (languageNameOverride && languageNameOverride[0])
        {
            strncpy_s(m_CurrentLanguage, sizeof(m_CurrentLanguage), languageNameOverride, _TRUNCATE);
        }
        else
        {
            const char* source = nullptr;
            if (!normalizedPhrases.empty())
            {
                source = normalizedPhrases.c_str();
            }
            else if (!normalizedAnswers.empty())
            {
                source = normalizedAnswers.c_str();
            }

            if (source)
            {
                const char* lastSlash = strrchr(source, '\\');
                if (lastSlash && lastSlash > source)
                {
                    const char* prevSlash = lastSlash - 1;
                    while (prevSlash > source && *prevSlash != '\\') prevSlash--;
                    if (*prevSlash == '\\') prevSlash++;

                    size_t len = lastSlash - prevSlash;
                    if (len > 0 && len < sizeof(m_CurrentLanguage))
                    {
                        memcpy(m_CurrentLanguage, prevSlash, len);
                        m_CurrentLanguage[len] = 0;
                    }
                }
            }
        }

        LogAdd(LOG_GREEN, "[PhraseManager] Loaded language: %s (Phrases: %s, Answers: %s)",
            m_CurrentLanguage, 
            m_PhrasesLoaded ? "OK" : "NOT FOUND",
            m_AnswersLoaded ? "OK" : "NOT FOUND");
        return true;
    }

    return false;
}

bool CPhraseManager::LoadPhrasesPath(const char* phrasesFolder)
{
    char filePath[260];
    sprintf_s(filePath, sizeof(filePath), "%sBotPhrases.txt", phrasesFolder);

    // Check if file exists
    DWORD attrib = GetFileAttributes(filePath);
    if (attrib == INVALID_FILE_ATTRIBUTES || (attrib & FILE_ATTRIBUTE_DIRECTORY))
    {
        LogAdd(LOG_RED, "[PhraseManager] BotPhrases.txt not found: %s", filePath);
        return false;
    }

    // Store the path
    sprintf_s(m_CurrentBotPhrasesPath, sizeof(m_CurrentBotPhrasesPath), "%s", filePath);
    LogAdd(LOG_GREEN, "[PhraseManager] Found BotPhrases.txt: %s", filePath);
    return true;
}

bool CPhraseManager::LoadAnswersPath(const char* answersFolder)
{
    char filePath[260];
    sprintf_s(filePath, sizeof(filePath), "%sAnswering.txt", answersFolder);

    // Check if file exists
    DWORD attrib = GetFileAttributes(filePath);
    if (attrib == INVALID_FILE_ATTRIBUTES || (attrib & FILE_ATTRIBUTE_DIRECTORY))
    {
        LogAdd(LOG_RED, "[PhraseManager] Answering.txt not found: %s", filePath);
        return false;
    }

    // Store the path
    sprintf_s(m_CurrentAnsweringPath, sizeof(m_CurrentAnsweringPath), "%s", filePath);
    LogAdd(LOG_GREEN, "[PhraseManager] Found Answering.txt: %s", filePath);
    return true;
}

void CPhraseManager::LoadDefaultPaths()
{
    // Fallback to old paths
    sprintf_s(m_CurrentLanguage, sizeof(m_CurrentLanguage), "Default");
    sprintf_s(m_CurrentBotPhrasesPath, sizeof(m_CurrentBotPhrasesPath), 
        ".\\IA\\Phrases\\BotPhrases.txt");
    sprintf_s(m_CurrentAnsweringPath, sizeof(m_CurrentAnsweringPath), 
        ".\\IA\\Answers\\Answering.txt");
    
    m_PhrasesLoaded = true;
    m_AnswersLoaded = true;

    LogAdd(LOG_BLUE, "[PhraseManager] Using default phrase/answer paths");
}

const char* CPhraseManager::GetBotPhrasesPath() const
{
    return m_CurrentBotPhrasesPath;
}

const char* CPhraseManager::GetAnsweringPath() const
{
    return m_CurrentAnsweringPath;
}

bool CPhraseManager::ReloadCurrent()
{
    if (m_CurrentLanguage[0] != 0 && strcmp(m_CurrentLanguage, "Default") != 0)
    {
        return LoadLanguage(m_CurrentLanguage);
    }
    return false;
}
