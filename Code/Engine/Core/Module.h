/**
* Module.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include <memory>


#ifndef CORE_EXPORTS
#define CORE_API __declspec (dllimport)
#else
#define CORE_API __declspec (dllexport)
#endif


typedef std::shared_ptr<class IScriptSystem> TScriptSystemPtr;
typedef std::shared_ptr<class IAudioSystem> TAudioSystemPtr;
typedef std::shared_ptr<class IPhysicsWorld> TPhysicsWorldPtr;

