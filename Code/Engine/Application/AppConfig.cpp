/**
* AppConfig.cpp
* 23.08.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "Application/AppConfig.h"
#include "Core/LCUtils.h"

#include "Json/include/nlohmann/json.hpp"
using json = nlohmann::json;


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

	json cfg = json::parse(fullConfigText);

	outConfig.WinWidth			= cfg["WinWidth"].get<unsigned int>();
	outConfig.WinHeight			= cfg["WinHeight"].get<unsigned int>();
	outConfig.bVSync			= cfg["bVSync"].get<bool>();
	outConfig.bAllowFullscreen	= cfg["bAllowFullscreen"].get<bool>();
	outConfig.bNoDelay			= cfg["bNoDelay"].get<bool>();

	return true;
}

void SaveConfig(const LCAppConfig& config, const char* fileName)
{
	json cfg = {
		{"WinWidth",			config.WinWidth},
		{"WinHeight",			config.WinHeight},
		{"bVSync",				config.bVSync},
		{"bAllowFullscreen",	config.bAllowFullscreen},
		{"bNoDelay",			config.bNoDelay},
	};

	WriteTextFile(fileName, cfg.dump(4) + "\n");
}
