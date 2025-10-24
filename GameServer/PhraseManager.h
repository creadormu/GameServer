// PhraseManager.h - Multi-Language Bot Phrases and Answers System
#pragma once

#include <string>

#define MAX_PHRASE_LANGUAGES 10

class CPhraseManager
{
public:
    CPhraseManager();
    ~CPhraseManager();

    void Initialize();

    // Language management (syncs with NameManager)
    bool LoadLanguage(const char* languageName);
    bool LoadLanguageFromPath(const char* phrasesFolder, const char* answersFolder);
    const char* GetCurrentLanguage() const { return m_CurrentLanguage; }
    bool HasPhrases() const { return m_PhrasesLoaded; }
    bool HasAnswers() const { return m_AnswersLoaded; }

    // Get file paths for current language
    const char* GetBotPhrasesPath() const;
    const char* GetAnsweringPath() const;

    // Reload current language (useful after editing files)
    bool ReloadCurrent();

private:
    bool LoadPhrasesPath(const char* phrasesFolder);
    bool LoadAnswersPath(const char* answersFolder);
    void LoadDefaultPaths(); // Fallback paths

    char m_PhrasesBasePath[260];
    char m_AnswersBasePath[260];
    char m_CurrentLanguage[50];

    // Current paths
    char m_CurrentBotPhrasesPath[260];
    char m_CurrentAnsweringPath[260];

    bool m_PhrasesLoaded;
    bool m_AnswersLoaded;
};

extern CPhraseManager g_PhraseManager;
