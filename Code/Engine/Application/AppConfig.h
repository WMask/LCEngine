/**
* AppConfig.h
* 23.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include <map>

#include "Module.h"
#include "Core/LCAny.h"


typedef std::map<std::string, LcAny> CONFIG_ITEMS;


/**
* Loads Application config from file */
APPLICATION_API CONFIG_ITEMS LoadConfig(const char* fileName = "config.txt", char delim = '\n');
