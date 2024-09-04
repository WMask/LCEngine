/**
* Fonts.h
* 04.09.2024
* (c) Denis Romakhov
*/

#include "GUI/Fonts.h"
#include "Core/LCException.h"

#include <string>
#include <vector>
#include <codecvt>

#include "Json/include/nlohmann/json.hpp"
using json = nlohmann::json;


void LcFont::Load(const LcPath& jsonPath, unsigned int textureId, std::wstring& outFontName)
{
	LC_TRY

	auto fullJsonText = ReadTextFile(jsonPath);
	if (fullJsonText.length() == 0)
	{
		throw LcException("Cannot read json");
	}

	json font = json::parse(fullJsonText);

	fontSize = font["fontSize"].get<unsigned int>();
	std::string utf8Name = font["fontName"].get<std::string>();
	outFontName = FromUtf8(utf8Name);

	json glyphsArray = font["glyphs"];
	if (glyphsArray.is_array())
	{
		for (auto& glyphIt : glyphsArray.items())
		{
			auto glyphObj = glyphIt.value();
			if (glyphObj["glyph"].is_string() &&
				glyphObj["pos"].is_object() &&
				glyphObj["size"].is_object())
			{
				std::string utf8Glyph = glyphObj["glyph"].get<std::string>();
				if (utf8Glyph.length() == 2)
				{
					if (utf8Glyph[0] == '\'') utf8Glyph = "\"";
					if (utf8Glyph[0] == '\\') utf8Glyph = "\\";
				}

				LcGlyph glyph{};
				glyph.pos.x = glyphObj["pos"]["x"].get<float>();
				glyph.pos.y = glyphObj["pos"]["y"].get<float>();
				glyph.size.x = glyphObj["size"]["x"].get<float>();
				glyph.size.y = glyphObj["size"]["y"].get<float>();
				glyph.textureId = textureId;
				std::wstring glyphName = FromUtf8(utf8Glyph);
				glyphs.insert({ glyphName[0], glyph });
			}
		}
	}

	LC_CATCH{ LC_THROW("LcFont::Load()") }
}

bool LcFont::FindGlyph(wchar_t glyphCode, LcGlyph& outGlyph) const
{
	auto glyphIt = glyphs.find(glyphCode);
	if (glyphIt != glyphs.end())
	{
		outGlyph = glyphIt->second;
		return true;
	}

	return false;
}

void LcFontManager::AddFont(const LcPath& jsonPath, unsigned int textureId)
{
	LC_TRY

	auto fullJsonText = ReadTextFile(jsonPath);
	if (fullJsonText.length() == 0)
	{
		throw LcException("Cannot read json");
	}

	json font = json::parse(fullJsonText);

	std::string utf8Name = font["fontName"].get<std::string>();
	std::wstring fontName = FromUtf8(utf8Name);

	auto fontIt = fonts.find(fontName);
	if (fontIt != fonts.end())
	{
		fontIt->second.Load(jsonPath, textureId, fontName);
	}
	else
	{
		LcFont newFont;
		newFont.Load(jsonPath, textureId, fontName);
		fonts.insert({ fontName, newFont });
	}

	LC_CATCH{ LC_THROW("LcFontManager::AddFont()") }
}

bool LcFontManager::FindGlyph(const std::wstring_view& fontName, wchar_t glyphCode, LcGlyph& outGlyph) const
{
	auto fontIt = fonts.find(fontName.data());
	if (fontIt != fonts.end())
	{
		return fontIt->second.FindGlyph(glyphCode, outGlyph);
	}

	return false;
}

bool LcFontManager::FindGlyphs(const std::wstring_view& fontName, const std::wstring_view& text, std::vector<LcGlyph>& outGlyphs, LcSizef* outTextSize) const
{
	std::string fontNameUtf8;

	LC_TRY

	auto fontIt = fonts.find(fontName.data());
	if (fontIt != fonts.end())
	{
		outGlyphs.clear();
		outGlyphs.reserve(text.length());

		float width = 0.0f, height = fontIt->second.GetFontSizeF();

		for (auto it(text.cbegin()); it != text.cend(); ++it)
		{
			LcGlyph glyph{};
			if (!fontIt->second.FindGlyph(*it, glyph))
			{
				fontNameUtf8 = ToUtf8(fontName.data());
				throw LcException("Cannot find glyph");
			}

			width += glyph.size.x;
			outGlyphs.push_back(glyph);
		}

		if (outTextSize) *outTextSize = { width, height };

		return outGlyphs.size() == text.length();
	}

	LC_CATCH{ LC_THROW_EX("LcFontManager::FindGlyphs('", fontNameUtf8.c_str(), "')") }

	return false;
}
