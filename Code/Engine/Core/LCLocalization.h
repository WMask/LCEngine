/**
* LCLocalization.h
* 29.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"

#include <string>
#include <map>

#pragma warning(disable : 4251)


/** Localization file */
class CORE_API LcLocalization
{
public:
	typedef std::map<std::string, std::wstring> TLocMap;
	//
	LcLocalization& operator=(const LcLocalization&) = default;
	//
	LcLocalization(const LcLocalization&) = default;


public:
	LcLocalization() {}
	//
	~LcLocalization() {}
	//
	void Load(const char* filePath);
	//
	std::wstring Get(const char* key) const;
	//
	std::string GetCulture() const { return culture; }


protected:
	std::string culture;
	//
	TLocMap entries;

};
