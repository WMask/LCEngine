/**
* LCLocalization.cpp
* 29.09.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "Core/LCLocalization.h"
#include "Core/LCException.h"
#include "Core/LCUtils.h"

#include "Json/include/nlohmann/json.hpp"
using json = nlohmann::json;

constexpr wchar_t default_value[] = L"default_value";


void LcLocalization::Load(const char* filePath)
{
    LC_TRY

    auto fileText = ReadTextFile(filePath);

    json locFile = json::parse(fileText);
    culture = locFile["culture"].get<std::string>();

    for (auto entry : locFile["entries"])
    {
        std::string key = entry["key"].get<std::string>();
        std::string utf8enc = entry["value"].get<std::string>();
        std::wstring value = FromUtf8(utf8enc);

        entries[key] = value;
    }

    LC_CATCH{ LC_THROW_EX("LcLocalization::Load('", filePath, "')"); }
}

std::wstring LcLocalization::Get(const char* key) const
{
    auto it = entries.find(key);
    return (it == entries.end()) ? default_value : it->second;
}

void LcLocalizationManager::Add(const char* filePath)
{
    LC_TRY

    auto newCulture = std::make_shared<LcLocalization>();
    newCulture->Load(filePath);

    auto cultureName = newCulture->GetCulture();
    bool cultureChanged = cultures.empty();

    cultures[cultureName] = newCulture;

    if (cultureChanged)
    {
        culture = cultureName;

        if (context) onCultureChanged.Broadcast(cultureName, *context);
    }

    LC_CATCH{ LC_THROW("LcLocalizationManager::Add()"); }
}

bool LcLocalizationManager::SetCulture(const char* inCulture)
{
    auto it = cultures.find(culture);
    if (it != cultures.end() &&
        culture != inCulture)
    {
        culture = inCulture;
        if (context) onCultureChanged.Broadcast(culture, *context);

        return true;
    }

    return false;
}

void LcLocalizationManager::Set(const char* key, const wchar_t* value)
{
    auto it = cultures.find(culture);
    if (it != cultures.end() && key && value)
    {
        it->second->Set(key, value);
    }
}

std::wstring LcLocalizationManager::Get(const char* key) const
{
    auto it = cultures.find(culture);
    return (it == cultures.end()) ? default_value : it->second->Get(key);
}
