/**
* Module.h
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once

#ifndef WORLD_EXPORTS
#define WORLD_API __declspec (dllimport)
#else
#define WORLD_API __declspec (dllexport)
#endif

#include "WorldInterface.h"


/**
* Get game world manager */
WORLD_API IWorld* GetWorld();
