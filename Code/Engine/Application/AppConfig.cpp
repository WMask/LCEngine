/**
* AppConfig.cpp
* 23.08.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "Application/AppConfig.h"
#include "Core/LCUtils.h"

#include <iostream>
#include <sstream>


CONFIG_ITEMS LoadConfig(const char* fileName, char delim)
{
	CONFIG_ITEMS items;

	auto fullConfigText = ReadTextFile(fileName);
	if (fullConfigText.length() < 3) return items;

	auto isBool = [](const std::string& str) {
		return str == "true" || str == "false";
	};

	auto isInt = [](const std::string& str) {
		for (auto c : str)
		{
			if (c == '-') continue;
			if (c < '0' || c > '9') return false;
		}
		return true;
	};

	auto isFloat = [](const std::string& str) {
		for (auto c : str)
		{
			if (c == '-' || c == '.') continue;
			if (c < '0' || c > '9') return false;
		}
		return true;
	};

	std::string line;
	std::stringstream cs(fullConfigText);
	while (std::getline(cs, line, delim))
	{
		if (line.length() < 3) continue;

		std::stringstream ls(line);
		std::string key;
		ls >> key;
		if (key.empty()) continue;
		if (key[0] == '#' || key[0] == '[') continue;

		std::string value;
		ls >> value;
		if (value.empty()) continue;

		LcAppConfigItem item;
		if (isBool(value)) item.bValue = (key == "true");
		else
		if (isInt(value)) item.iValue = atoi(value.c_str());
		else
		if (isFloat(value)) item.fValue = (float)atof(value.c_str());
		else item.sValue = value;

		items[key] = item;
	}

	return items;
}
