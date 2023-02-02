/**
* Module.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once


#ifndef CORE_EXPORTS
#define CORE_API __declspec (dllimport)
#else
#define CORE_API __declspec (dllexport)
#endif
