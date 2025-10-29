// NameManager.h - Multi-Language Name Loading System
#pragma once

#include <string>
#include <vector>

#define MAX_NAME_LENGTH 32
#define MAX_LANGUAGES 10

class CNameManager
{
public:
    CNameManager();
    ~CNameManager();

    void Initialize();

    // Language management
    bool LoadLanguage(const char* languageName);
    bool LoadLanguageFromPath(const char* folderPath);
    int GetAvailableLanguageCount() const { return m_AvailableLanguageCount; }
    const char* GetCurrentLanguage() const { return m_CurrentLanguage; }
    const char* GetLanguageName(int index) const;

    // Name retrieval
    const char* GetRandomMaleName();
    const char* GetRandomFemaleName();
    int GetMaleNameCount() const { return m_MaleNameCount; }
    int GetFemaleNameCount() const { return m_FemaleNameCount; }

    // Utility
    void ClearNames();
    bool HasNames() const { return (m_MaleNameCount > 0 || m_FemaleNameCount > 0); }

private:
    bool LoadNamesFromFile(const char* filePath, bool isMale);
    bool ScanAvailableLanguages();
    void LoadDefaultNames(); // Fallback if no files found

    char m_BasePath[260];
    char m_CurrentLanguage[50];

    // Dynamic name storage
    std::vector<std::string> m_MaleNames;
    std::vector<std::string> m_FemaleNames;
    int m_MaleNameCount;
    int m_FemaleNameCount;

    // Available languages
    char m_AvailableLanguages[MAX_LANGUAGES][50];
    int m_AvailableLanguageCount;
};

extern CNameManager g_NameManager;