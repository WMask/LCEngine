/**
* Module.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include <memory>


#ifdef _WINDOWS

#ifndef CORE_EXPORTS
#define CORE_API __declspec (dllimport)
#else
#define CORE_API __declspec (dllexport)
#endif

#else

#define CORE_API __attribute__((visibility("default")))

#endif


typedef std::shared_ptr<class IScriptSystem> TScriptSystemPtr;
typedef std::shared_ptr<class IAudioSystem> TAudioSystemPtr;
typedef std::shared_ptr<class IInputSystem> TInputSystemPtr;
typedef std::shared_ptr<class IPhysicsWorld> TPhysicsWorldPtr;

