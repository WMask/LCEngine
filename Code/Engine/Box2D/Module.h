/**
* Module.h
* 08.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Core/Physics.h"


#ifndef BOX2D_EXPORTS
#define BOX2D_API __declspec (dllimport)
#else
#define BOX2D_API __declspec (dllexport)
#endif


/** Box2D physics system */
BOX2D_API TPhysicsWorldPtr GetPhysicsWorld();
