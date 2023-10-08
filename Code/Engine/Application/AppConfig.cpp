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


LcAppConfig::LcAppConfig()
{
	WinWidth = 800;
	WinHeight = 600;
	bVSync = true;
	bAllowFullscreen = false;
	bNoDelay = false;
}

bool LoadConfig(LcAppConfig& outConfig, const char* fileName, char delim)
{
	auto fullConfigText = ReadTextFile(fileName);
	if (fullConfigText.length() < 3) return false;

	json cfg = json::parse(fullConfigText);

	outConfig.WinWidth			= cfg["Application"]["WinWidth"].get<unsigned int>();
	outConfig.WinHeight			= cfg["Application"]["WinHeight"].get<unsigned int>();
	outConfig.bVSync			= cfg["Render"]["bVSync"].get<bool>();
	outConfig.bAllowFullscreen	= cfg["Render"]["bAllowFullscreen"].get<bool>();
	outConfig.bNoDelay			= cfg["Render"]["bNoDelay"].get<bool>();

	for (auto action : cfg["Actions"])
	{
		LcActionBinding binding{};
		binding.Name     = action["Name"].get<std::string>();
		binding.Key      = action["Key"].get<int>();
		binding.JoyKey   = action["JoyKey"].get<int>();
		binding.MouseBtn = action["MouseBtn"].get<int>();
		binding.AxisId   = action["AxisId"].get<int>();
		outConfig.Actions.push_back(binding);
	}

	return true;
}

void SaveConfig(const LcAppConfig& config, const char* fileName)
{
	json cfg = {
		{"Application", {
			{"WinWidth",			config.WinWidth},
			{"WinHeight",			config.WinHeight}
		}},
		{"Render", {
			{"bVSync",				config.bVSync},
			{"bAllowFullscreen",	config.bAllowFullscreen},
			{"bNoDelay",			config.bNoDelay}
		}},
		{"Actions", {}}
	};

	for (auto& action : config.Actions)
	{
		json binding = {
			{"Name", action.Name},
			{"Key", action.Key},
			{"JoyKey", action.JoyKey},
			{"MouseBtn", action.MouseBtn},
			{"AxisId", action.AxisId},
		};
		cfg["Actions"].push_back(binding);
	}

	WriteTextFile(fileName, cfg.dump(4) + "\n");
}
