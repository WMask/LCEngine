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
    return (it == entries.end()) ? L"default_text" : it->second;
}
