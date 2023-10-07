/**
* LCTypes.cpp
* 24.08.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "Core/LCTypes.h"

#include <algorithm>


float LcClamp(float value, float minValue, float maxValue)
{
	return std::clamp(value, minValue, maxValue);
}

int LcClamp(int value, int minValue, int maxValue)
{
	return std::clamp(value, minValue, maxValue);
}
