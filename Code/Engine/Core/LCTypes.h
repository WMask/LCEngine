/**
* LCTypes.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"


/** Mouse buttons */
enum class LcMouseBtn
{
	Left,
	Right,
	Middle
};


/** Key state */
enum class LcKeyState
{
	Down,
	Up
};


/**
* Render system type */
enum class LcRenderSystemType
{
	Null,
	DX7,
	DX9,
	DX10
};


/**
* Keys struct */
struct CORE_API KEYS
{
	KEYS();
	//
	unsigned char& operator[](int index);
	//
	const static int numKeys = 128;
	//
	unsigned char keys[numKeys];
};
