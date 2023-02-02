/**
* Module.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Application/Application.h"


#ifndef WINDOWSAPPLICATION_EXPORTS
#define WINDOWSAPPLICATION_API __declspec (dllimport)
#else
#define WINDOWSAPPLICATION_API __declspec (dllexport)
#endif


WINDOWSAPPLICATION_API std::shared_ptr<IApplication> GetApp();
