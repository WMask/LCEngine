/**
* Module.h
* 08.09.2023
* (c) Denis Romakhov
*/

#pragma once


#ifndef BOX2D_EXPORTS
#define BOX2D_API __declspec (dllimport)
#else
#define BOX2D_API __declspec (dllexport)
#endif
