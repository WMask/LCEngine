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


typedef std::unique_ptr<class IRenderSystem> TRenderSystemPtr;
