/**
* Module.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once


#ifdef _WINDOWS

#ifndef APPLICATION_EXPORTS
#define APPLICATION_API __declspec (dllimport)
#else
#define APPLICATION_API __declspec (dllexport)
#endif

#elif __APPLE__

#define APPLICATION_API 

#endif
