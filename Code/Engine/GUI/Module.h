/**
* Module.h
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include <memory>


#ifndef GUI_EXPORTS
#define GUI_API __declspec (dllimport)
#else
#define GUI_API __declspec (dllexport)
#endif


typedef std::unique_ptr<class IGuiManager> TGuiManagerPtr;
