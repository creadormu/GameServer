#pragma once

#include <string>
#include <vector>
#include <iosfwd>

struct LanguageConfigEntry
{
    std::string name;
    std::string namesDir;
    std::string phrasesDir;
    std::string answersDir;

    bool HasNames() const { return !namesDir.empty(); }
    bool HasPhrases() const { return !phrasesDir.empty(); }
    bool HasAnswers() const { return !answersDir.empty(); }
};

class LanguageConfig
{
public:
    LanguageConfig();

    bool Load(const char* configPath);
    bool Reload();

    bool IsLoaded() const { return m_loaded; }
    const std::string& GetConfigPath() const { return m_configPath; }

    size_t GetLanguageCount() const { return m_languages.size(); }
    const std::vector<LanguageConfigEntry>& GetLanguages() const { return m_languages; }

    const LanguageConfigEntry* GetLanguage(size_t index) const;
    const LanguageConfigEntry* FindLanguage(const char* name) const;

private:
    bool ParseStream(std::istream& input);
    static std::string Trim(const std::string& value);
    static std::string NormalizePath(const std::string& value, bool ensureTrailingSlash);

private:
    std::string m_configPath;
    std::vector<LanguageConfigEntry> m_languages;
    bool m_loaded;
};

extern LanguageConfig g_LanguageConfig;
