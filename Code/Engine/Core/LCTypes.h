/**
* LCTypes.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

// copy Eigen code to LCEngine/Code/Engine/Core/Eigen
#include "Eigen/Dense"


/** Geometry */
typedef Eigen::Vector2i LcSize;
typedef Eigen::Vector2f LcSizef;
typedef Eigen::Vector2f LcVector2;
typedef Eigen::Vector3f LcVector3;
typedef Eigen::Vector4f LcVector4;
typedef Eigen::Vector4f LcColor4;
typedef Eigen::Matrix4f LcMatrix4;
typedef Eigen::AlignedBox<int, 2>	LcRect;
typedef Eigen::AlignedBox<float, 2>	LcRectf;


/** Mouse buttons */
enum class LcMouseBtn
{
	Left,
	Right,
	Middle
};


/** Key state */
enum class LcKeyState
{
	Down,
	Up
};


/**
* Render system type */
enum class LcRenderSystemType
{
	Null,
	DX7,
	DX9,
	DX10
};
