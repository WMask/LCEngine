/**
* Fonts.h
* 04.09.2024
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/LCTypesEx.h"
#include "Core/LCUtils.h"

#include <string>
#include <string_view>
#include <array>
#include <map>

#pragma warning(disable : 4251)
#pragma warning(disable : 5046)


/**
* Font glyph */
struct LcGlyph
{
	// texture id
	unsigned int textureId;
	// position on texture in pixels
	LcVector2 pos;
	// size on texture in pixels
	LcSizef size;
};


/**
* Font class */
class LcFont
{
public:
	//
	LcFont() : fontSize(0) {}
	//
	void Load(const LcPath& jsonPath, unsigned int textureId);
	//
	bool FindGlyph(wchar_t glyphCode, LcGlyph& outGlyph) const;
	//
	inline bool HasGlyph(wchar_t glyphCode) const { return glyphs.find(glyphCode) != glyphs.end(); }
	//
	inline size_t GetGlyphsCount() const { return glyphs.size(); }
	//
	inline std::wstring_view GetDisplayName() const { return displayName; }
	//
	inline float GetFontSizeF() const { return static_cast<float>(fontSize); }
	//
	inline unsigned int GetFontSize() const { return fontSize; }


protected:
	//
	std::map<wchar_t, LcGlyph> glyphs;
	//
	std::wstring displayName;
	//
	unsigned int fontSize;

};


/**
* Font manager */
class GUI_API LcFontManager
{
public:
	//
	LcFontManager() {}
	//
	void AddFont(const LcPath& jsonPath, const std::string_view& fontName, unsigned int textureId);
	//
	bool FindGlyph(const std::string_view& fontName, wchar_t glyphCode, LcGlyph& outGlyph) const;
	/*
	* outTextSize - full string size in pixels
	*/
	bool FindGlyphs(const std::string_view& fontName, const std::wstring_view& text, std::vector<LcGlyph>& outGlyphs, LcSizef* outTextSize) const;
	/*
	* requiredSize - requires line height in pixels
	* outTextSize - full string size in pixels
	*/
	bool FindGlyphsScaled(const std::string_view& fontName, const std::wstring_view& text, float requiredSize, std::vector<LcGlyph>& outGlyphs, LcSizef* outTextSize) const;


protected:
	//
	const LcFont* GetBestFontLessThan(const std::wstring_view& displayName, float requiredSize) const;


protected:
	// key - font name
	std::map<std::string, LcFont> fonts;

};
