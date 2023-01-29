/**
* LCUtils.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "CoreModule.h"
#include "LCTypes.h"


/**
* Create ortho matrix */
CORE_API Eigen::Matrix4f OrthoMatrix(float left, float right, float bottom, float top, float nearPlane, float farPlane);
/**
* Create ortho matrix */
CORE_API Eigen::Matrix4f OrthoMatrix(LCSize viewportSize, float nearPlane, float farPlane);
/**
* Create translation matrix */
CORE_API Eigen::Matrix4f TranslationMatrix(Eigen::Vector3f pos);
/**
* Create rotation matrix */
CORE_API Eigen::Matrix4f TransformMatrix(Eigen::Vector3f pos, Eigen::Vector2f scale, float rotZ = 0.0f);


/**
* Read text file */
CORE_API std::string ReadTextFile(const std::string& filePath);


/**
* Print debug string */
CORE_API void DebugMsg(const char* fmt, ...);
/**
* Print debug string */
CORE_API void DebugMsgW(const wchar_t* fmt, ...);
