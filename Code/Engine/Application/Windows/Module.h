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


typedef std::unique_ptr<class IApplication> TAppPtr;


/**
* Windows application */
WINDOWSAPPLICATION_API TAppPtr GetApp();

/**
* Windows DirectInput system */
WINDOWSAPPLICATION_API TInputSystemPtr GetInputSystem();
