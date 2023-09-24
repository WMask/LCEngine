/**
* LCUtils.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "LCTypes.h"


/**
* Read text file */
CORE_API std::string ReadTextFile(const char* filePath);
/**
* Read binary file */
CORE_API LcBytes ReadBinaryFile(const char* filePath);
/**
* Write text file */
CORE_API void WriteTextFile(const char* filePath, const std::string& text);


/**
* Print debug string */
CORE_API void DebugMsg(const char* fmt, ...);
/**
* Print debug string */
CORE_API void DebugMsgW(const wchar_t* fmt, ...);


/**
* Show message dialog and wait */
CORE_API void ShowMessageModal(const char* message, const char* title);


/**
* To lower case */
CORE_API std::string ToLower(const char* str);
/**
* To upper case */
CORE_API std::string ToUpper(const char* str);
/**
* To string */
CORE_API std::string ToString(int value);
/**
* To string */
CORE_API std::string ToString(float value);
/**
* To string */
CORE_API std::wstring ToStringW(int value);
/**
* To string */
CORE_API std::wstring ToStringW(float value);
