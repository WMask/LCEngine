/**
* Module.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "RenderSystem/RenderSystem.h"

#include <memory>


#ifndef RENDERSYSTEMDX10_EXPORTS
#define RENDERSYSTEMDX10_API __declspec (dllimport)
#else
#define RENDERSYSTEMDX10_API __declspec (dllexport)
#endif


/**
* DirectX 10 render system */
RENDERSYSTEMDX10_API TRenderSystemPtr GetRenderSystem();
