#include "stdafx.h"
#include "LanguageConfig.h"
#include "Util.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>

LanguageConfig g_LanguageConfig;

LanguageConfig::LanguageConfig()
    : m_loaded(false)
{
}

bool LanguageConfig::Load(const char* configPath)
{
    m_languages.clear();
    m_loaded = false;

    if (configPath == nullptr || configPath[0] == 0)
    {
        LogAdd(LOG_RED, (char*)"[LanguageConfig] Invalid config path");
        return false;
    }

    m_configPath = configPath;

    std::ifstream file(configPath);
    if (!file.is_open())
    {
        LogAdd(LOG_RED, (char*)"[LanguageConfig] Config file not found: %s", configPath);
        return false;
    }

    if (!ParseStream(file))
    {
        LogAdd(LOG_RED, (char*)"[LanguageConfig] Failed to parse %s", configPath);
        return false;
    }

    if (m_languages.empty())
    {
        LogAdd(LOG_RED, (char*)"[LanguageConfig] No languages defined in %s", configPath);
        return false;
    }

    m_loaded = true;

    LogAdd(LOG_GREEN, (char*)"[LanguageConfig] Loaded %d language(s) from %s", (int)m_languages.size(), configPath);
    for (size_t i = 0; i < m_languages.size(); ++i)
    {
        LogAdd(LOG_BLACK, "  - %s", m_languages[i].name.c_str());
    }

    return true;
}

bool LanguageConfig::Reload()
{
    if (m_configPath.empty())
    {
        return false;
    }

    return Load(m_configPath.c_str());
}

const LanguageConfigEntry* LanguageConfig::GetLanguage(size_t index) const
{
    if (index < m_languages.size())
    {
        return &m_languages[index];
    }
    return nullptr;
}

const LanguageConfigEntry* LanguageConfig::FindLanguage(const char* name) const
{
    if (name == nullptr || name[0] == 0)
    {
        return nullptr;
    }

    for (size_t i = 0; i < m_languages.size(); ++i)
    {
        if (_stricmp(m_languages[i].name.c_str(), name) == 0)
        {
            return &m_languages[i];
        }
    }

    return nullptr;
}

bool LanguageConfig::ParseStream(std::istream& input)
{
    std::string line;
    LanguageConfigEntry current;
    bool insideSection = false;

    while (std::getline(input, line))
    {
        std::string trimmed = Trim(line);

        if (trimmed.empty() || trimmed[0] == ';')
        {
            continue;
        }

        if (trimmed.front() == '[' && trimmed.back() == ']')
        {
            if (insideSection && !current.name.empty())
            {
                m_languages.push_back(current);
            }

            current = LanguageConfigEntry();
            insideSection = false;

            std::string section = trimmed.substr(1, trimmed.length() - 2);
            std::string sectionTrimmed = Trim(section);

            const char* prefix = "Language";
            size_t prefixLen = strlen(prefix);

            if (_strnicmp(sectionTrimmed.c_str(), prefix, prefixLen) == 0)
            {
                size_t separator = sectionTrimmed.find_first_of(": ");
                if (separator == std::string::npos)
                {
                    separator = prefixLen;
                }

                std::string namePart;

                if (separator < sectionTrimmed.length())
                {
                    namePart = sectionTrimmed.substr(separator + 1);
                }

                namePart = Trim(namePart);

                if (!namePart.empty())
                {
                    current.name = namePart;
                    insideSection = true;
                }
            }

            continue;
        }

        if (!insideSection)
        {
            continue;
        }

        size_t equalsPos = trimmed.find('=');
        if (equalsPos == std::string::npos)
        {
            continue;
        }

        std::string key = Trim(trimmed.substr(0, equalsPos));
        std::string value = Trim(trimmed.substr(equalsPos + 1));

        if (key.empty() || value.empty())
        {
            continue;
        }

        if (_stricmp(key.c_str(), "NamesDir") == 0 || _stricmp(key.c_str(), "Names") == 0)
        {
            current.namesDir = NormalizePath(value, true);
        }
        else if (_stricmp(key.c_str(), "PhrasesDir") == 0 || _stricmp(key.c_str(), "Phrases") == 0)
        {
            current.phrasesDir = NormalizePath(value, true);
        }
        else if (_stricmp(key.c_str(), "AnswersDir") == 0 || _stricmp(key.c_str(), "Answers") == 0)
        {
            current.answersDir = NormalizePath(value, true);
        }
    }

    if (insideSection && !current.name.empty())
    {
        m_languages.push_back(current);
    }

    return true;
}

std::string LanguageConfig::Trim(const std::string& value)
{
    if (value.empty())
    {
        return std::string();
    }

    size_t start = value.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
    {
        return std::string();
    }

    size_t end = value.find_last_not_of(" \t\r\n");
    return value.substr(start, end - start + 1);
}

std::string LanguageConfig::NormalizePath(const std::string& value, bool ensureTrailingSlash)
{
    std::string path = Trim(value);

    std::replace(path.begin(), path.end(), '/', '\\');

    if (ensureTrailingSlash && !path.empty())
    {
        char last = path[path.length() - 1];
        if (last != '\\' && last != '/')
        {
            path += "\\";
        }
        else if (last == '/')
        {
            path[path.length() - 1] = '\\';
        }
    }

    return path;
}
