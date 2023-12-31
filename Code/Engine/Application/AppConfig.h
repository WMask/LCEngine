/**
* AppConfig.h
* 23.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include <map>

#include "Module.h"
#include "Core/LCTypes.h"

#pragma warning(disable : 4251)


struct LcActionBinding
{
    std::string Name;
    //
    int Key;
    //
    int JoyKey;
    //
    int MouseBtn;
    //
    int AxisId;
};

struct APPLICATION_API LcAppConfig
{
    LcAppConfig();

    // [Application]
    unsigned int WinWidth;
    unsigned int WinHeight;
    // [Engine]
    bool bVSync;
    bool bAllowFullscreen;
    bool bNoDelay;
    // [Input]
    std::deque<LcActionBinding> Actions;
};


/**
* Loads Application config from file */
APPLICATION_API bool LoadConfig(LcAppConfig& outConfig, const char* fileName = "config.txt", char delim = '\n');

/**
* Loads Application config from file */
APPLICATION_API void SaveConfig(const LcAppConfig& config, const char* fileName = "config.txt");
