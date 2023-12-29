/**
* Module.h
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include <memory>


#ifdef _WINDOWS

#ifndef GUI_EXPORTS
#define GUI_API __declspec (dllimport)
#else
#define GUI_API __declspec (dllexport)
#endif

#elif __APPLE__

#define GUI_API 

#endif


typedef std::shared_ptr<class IGuiManager> TGuiManagerPtr;
typedef std::weak_ptr<class IGuiManager> TWeakGuiManager;


GUI_API TGuiManagerPtr GetGuiManager();
