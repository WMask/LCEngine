/**
* LCUtils.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "LCTypes.h"

#include <string>
#include <vector>


/**
* Read text file */
CORE_API std::string ReadTextFile(const char* filePath);
/**
* Read binary file */
CORE_API std::vector<char> ReadBinaryFile(const char* filePath);


/**
* Print debug string */
CORE_API void DebugMsg(const char* fmt, ...);
/**
* Print debug string */
CORE_API void DebugMsgW(const wchar_t* fmt, ...);
