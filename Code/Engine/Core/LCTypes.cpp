/**
* LCTypes.cpp
* 24.08.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "Core/LCTypes.h"


KEYS::KEYS()
{
	memset(keys, 0, sizeof(keys));
}

unsigned char& KEYS::operator[](int index)
{
	if (index < 0 || index >= numKeys) throw std::exception("KEYS::operator[]: Invalid index");

	return keys[index];
}