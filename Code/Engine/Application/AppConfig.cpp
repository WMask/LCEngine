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

#ifdef _WINDOWS
# include <stdlib.h>
#endif

LCAppConfig::LCAppConfig()
{
	WinWidth = 800;
	WinHeight = 600;
	bVSync = true;
	bAllowFullscreen = false;
	bNoDelay = false;
}

bool LoadConfig(LCAppConfig& outConfig, const char* fileName, char delim)
{
	auto fullConfigText = ReadTextFile(fileName);
	if (fullConfigText.length() < 3) return false;

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
		bool dotFound = false;
		for (auto c : str)
		{
			if (c == '-') continue;
			if (c == '.') { dotFound = true; continue; }
			if (c < '0' || c > '9') return false;
		}
		return dotFound;
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
		if (key[0] == '#' || key[0] == '*' || key[0] == '/' || key[0] == '[') continue;

		std::string value;
		ls >> value;
		if (value.empty()) continue;

		LcAny item;

		if (isBool(value)) item = LcAny(value == "true");
#ifdef _WINDOWS
		else
		if (isFloat(value)) item = LcAny((float)atof(value.c_str()));
		else
		if (isInt(value)) item = LcAny(atoi(value.c_str()));
#endif
		else item = LcAny(value);

		if (false)
			return false;
		else if (key == "WinWidth")
			outConfig.WinWidth = item.iValue;
		else if (key == "WinHeight")
			outConfig.WinHeight = item.iValue;
		else if (key == "bVSync")
			outConfig.bVSync = item.bValue;
		else if (key == "bAllowFullscreen")
			outConfig.bVSync = item.bValue;
		else if (key == "bNoDelay")
			outConfig.bNoDelay = item.bValue;
	}

	return true;
}

void SaveConfig(const LCAppConfig& config, const char* fileName)
{
	std::stringstream output;

	output << "[Application]\n";
	output << "WinWidth            " << config.WinWidth << '\n';
	output << "WinHeight           " << config.WinHeight << '\n';

	output << "\n[Render]\n";
	output << "bVSync              " << (config.bVSync ? "true" : "false") << '\n';
	output << "bAllowFullscreen    " << (config.bAllowFullscreen ? "true" : "false") << '\n';
	output << "bNoDelay            " << (config.bNoDelay ? "true" : "false") << '\n';

	WriteTextFile(fileName, output.str());
}
