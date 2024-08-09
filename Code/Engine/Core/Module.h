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


typedef std::unique_ptr<class IScriptSystem> TScriptSystemPtr;
typedef std::unique_ptr<class IAudioSystem> TAudioSystemPtr;
typedef std::unique_ptr<class IInputSystem> TInputSystemPtr;
typedef std::unique_ptr<class IPhysicsWorld> TPhysicsWorldPtr;
