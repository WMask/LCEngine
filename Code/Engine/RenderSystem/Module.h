/**
* Module.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include <memory>


#ifndef RENDERSYSTEM_EXPORTS
#define RENDERSYSTEM_API __declspec (dllimport)
#else
#define RENDERSYSTEM_API __declspec (dllexport)
#endif


typedef std::shared_ptr<class IRenderSystem> TRenderSystemPtr;
typedef std::weak_ptr<class IRenderSystem> TWeakRenderSystem;
