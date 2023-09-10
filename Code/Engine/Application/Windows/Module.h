/**
* Module.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Application/ApplicationInterface.h"


#ifndef WINDOWSAPPLICATION_EXPORTS
#define WINDOWSAPPLICATION_API __declspec (dllimport)
#else
#define WINDOWSAPPLICATION_API __declspec (dllexport)
#endif


typedef std::shared_ptr<class IApplication> TAppPtr;
typedef std::weak_ptr<class IApplication> TWeakApp;


/**
* Windows application */
WINDOWSAPPLICATION_API TAppPtr GetApp();
