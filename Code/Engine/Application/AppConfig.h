/**
* AppConfig.h
* 23.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include <map>
#include <string>

#include "Module.h"


/**
* Application config item */
struct LcAppConfigItem
{
	LcAppConfigItem() : fValue(0.0f), iValue(0), bValue(false) {}
	std::string	sValue;
	float		fValue;
	bool		bValue;
	int			iValue;
};

typedef std::map<std::string, LcAppConfigItem> CONFIG_ITEMS;


/**
* Loads Application config from file */
APPLICATION_API CONFIG_ITEMS LoadConfig(const char* fileName = "config.txt", char delim = '\n');
