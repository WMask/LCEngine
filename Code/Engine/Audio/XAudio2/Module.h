/**
* Module.h
* 18.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Core/Audio.h"


#ifndef XAUDIO2_EXPORTS
#define XAUDIO2_API __declspec (dllimport)
#else
#define XAUDIO2_API __declspec (dllexport)
#endif


/** Windows XAudio2 system */
XAUDIO2_API TAudioSystemPtr GetAudioSystem();
