/**
* LCTypes.cpp
* 24.08.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "Core/LCTypes.h"

#include <algorithm>


KEYS::KEYS()
{
	memset(keys, 0, sizeof(keys));
}

unsigned char& KEYS::operator[](int index)
{
	if (index < 0 || index >= numKeys) throw std::exception("KEYS::operator[]: Invalid index");

	return keys[index];
}

float LcClamp(float value, float minValue, float maxValue)
{
	return std::clamp(value, minValue, maxValue);
}

int LcClamp(int value, int minValue, int maxValue)
{
	return std::clamp(value, minValue, maxValue);
}
