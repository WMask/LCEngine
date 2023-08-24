/**
* LCTypes.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once


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
