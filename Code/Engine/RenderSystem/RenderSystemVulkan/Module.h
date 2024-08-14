/**
* Module.h
* 14.08.2024
* (c) Denis Romakhov
*/

#pragma once

#include "RenderSystem/RenderSystem.h"


#ifndef RENDERSYSTEMVULKAN_EXPORTS
#define RENDERSYSTEMVULKAN_API __declspec (dllimport)
#else
#define RENDERSYSTEMVULKAN_API __declspec (dllexport)
#endif


/**
* Vulkan render system */
RENDERSYSTEMVULKAN_API TRenderSystemPtr GetRenderSystem();
