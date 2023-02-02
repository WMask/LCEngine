/**
* Module.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "RenderSystem/RenderSystem.h"


#ifndef DX10RENDERSYSTEM_EXPORTS
#define DX10RENDERSYSTEM_API __declspec (dllimport)
#else
#define DX10RENDERSYSTEM_API __declspec (dllexport)
#endif


DX10RENDERSYSTEM_API std::shared_ptr<IRenderSystem> GetRenderSystem();
