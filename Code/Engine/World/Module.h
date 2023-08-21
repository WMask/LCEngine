/**
* Module.h
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include <memory>


#ifndef WORLD_EXPORTS
#define WORLD_API __declspec (dllimport)
#else
#define WORLD_API __declspec (dllexport)
#endif


typedef std::shared_ptr<class IWorld> TWorldPtr;
typedef std::weak_ptr<class IWorld> TWorldWeakPtr;


/**
* Get game world manager */
WORLD_API TWorldPtr GetWorld();
