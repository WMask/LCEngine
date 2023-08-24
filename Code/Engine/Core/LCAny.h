/**
* LCAny.h
* 24.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include <string>

/**
* Any value container */
struct LcAny
{
	LcAny() : fValue(0.0f), iValue(0), bValue(false) {}
	std::string	sValue;
	float		fValue;
	bool		bValue;
	int			iValue;
};
