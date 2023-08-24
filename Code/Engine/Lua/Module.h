/**
* Module.h
* 24.08.2023
* (c) Denis Romakhov
*/

#pragma once


#ifndef LUA_EXPORTS
#define LCLUA_API __declspec (dllimport)
#else
#define LCLUA_API __declspec (dllexport)
#endif
