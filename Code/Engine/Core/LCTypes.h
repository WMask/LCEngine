/**
* LCTypes.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Eigen/Dense"


/** Geometry */
typedef Eigen::Vector2i LcSize;
typedef Eigen::Vector2f LcSizef;
typedef Eigen::Vector2f LcVector2;
typedef Eigen::Vector3f LcVector3;
typedef Eigen::Vector4f LcVector4;
typedef Eigen::Vector4f LcColor4;
typedef Eigen::Matrix4f LcMatrix4;


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
