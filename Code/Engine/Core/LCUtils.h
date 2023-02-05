/**
* LCUtils.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "LCTypes.h"


/** Convert */
inline LcSizef ToF(const LcSize& size) { return LcSizef((float)size.x(), (float)size.y()); }
inline LcSize  ToI(const LcSizef& size) { return LcSize((int)size.x(), (int)size.y()); }
       LcRectf ToF(const LcRect& rect);
       LcRect  ToI(const LcRectf& rect);


/**
* Create ortho matrix */
CORE_API LcMatrix4 OrthoMatrix(float left, float right, float bottom, float top, float nearPlane, float farPlane);
/**
* Create ortho matrix */
CORE_API LcMatrix4 OrthoMatrix(LcSize viewportSize, float nearPlane, float farPlane);
/**
* Create translation matrix */
CORE_API LcMatrix4 TranslationMatrix(LcVector3 pos);
/**
* Create rotation matrix */
CORE_API LcMatrix4 TransformMatrix(LcVector3 pos, LcVector2 scale, float rotZ = 0.0f);


/**
* Read text file */
CORE_API std::string ReadTextFile(const std::string& filePath);


/**
* Print debug string */
CORE_API void DebugMsg(const char* fmt, ...);
/**
* Print debug string */
CORE_API void DebugMsgW(const wchar_t* fmt, ...);
