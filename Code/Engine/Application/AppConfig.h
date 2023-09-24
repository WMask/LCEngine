/**
* AppConfig.h
* 23.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include <map>

#include "Module.h"
#include "Core/LCTypes.h"


typedef std::map<std::string, LcAny> CONFIG_ITEMS;

struct APPLICATION_API LCAppConfig
{
    LCAppConfig();

    // [Application]
    unsigned int WinWidth;
    unsigned int WinHeight;
    // [Render]
    bool bVSync;
    bool bAllowFullscreen;
    bool bNoDelay;
};


/**
* Loads Application config from file */
APPLICATION_API bool LoadConfig(LCAppConfig& outConfig, const char* fileName = "config.txt", char delim = '\n');

/**
* Loads Application config from file */
APPLICATION_API void SaveConfig(const LCAppConfig& config, const char* fileName = "config.txt");
