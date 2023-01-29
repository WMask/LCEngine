/**
* WindowsApplicationModule.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once


#ifndef WINDOWSAPPLICATION_EXPORTS
#define WINDOWSAPPLICATION_API __declspec (dllimport)
#else
#define WINDOWSAPPLICATION_API __declspec (dllexport)
#endif
