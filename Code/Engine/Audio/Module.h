/**
* Module.h
* 18.09.2023
* (c) Denis Romakhov
*/

#pragma once


#ifndef AUDIO_EXPORTS
#define AUDIO_API __declspec (dllimport)
#else
#define AUDIO_API __declspec (dllexport)
#endif
