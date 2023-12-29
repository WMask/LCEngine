/**
* Module.h
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include <memory>


#ifdef _WINDOWS

#ifndef WORLD_EXPORTS
#define WORLD_API __declspec (dllimport)
#else
#define WORLD_API __declspec (dllexport)
#endif

#elif __APPLE__

#define WORLD_API

#endif


typedef std::shared_ptr<class IWorld> TWorldPtr;


/**
* Get game world manager */
WORLD_API TWorldPtr GetWorld(struct LcAppContext& context);
