/**
* LCUtils.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "LCTypes.h"

#include <string>
#include <vector>


/** Defaults */
namespace LcDefaults
{
	extern CORE_API LcVector3 OneVec3;
	extern CORE_API LcVector3 ZeroVec3;
	extern CORE_API LcVector4 OneVec4;
	extern CORE_API LcVector4 ZeroVec4;
};


/** Convert */
inline      LcSizef		ToF(const LcSize& size) { return LcSizef((float)size.x, (float)size.y); }
inline      LcSize		ToI(const LcSizef& size) { return LcSize((int)size.x, (int)size.y); }
inline      LcVector2	To2(const LcVector3& v) { return LcVector2(v.x, v.y); }
inline      LcVector3	To3(const LcVector2& v) { return LcVector3(v.x, v.y, 0.0f); }
CORE_API    LcVector4	ToV(const LcColor4& v);
CORE_API    LcColor4	ToC(const LcVector4& v);
CORE_API    LcRectf		ToF(const LcRect& rect);
CORE_API    LcRectf		ToF(const LcVector2& leftTop, const LcVector2& rightBottom);
CORE_API	LcRect		ToI(const LcRectf& rect);


/** Geometry */
inline bool Contains(const LcRectf& r, const LcVector2& p)
{
	return p.x >= r.left && p.x < r.right && p.y >= r.top && p.y < r.bottom;
}
inline bool Contains(const LcRect& r, const LcPoint& p)
{
	return p.x >= r.left && p.x < r.right && p.y >= r.top && p.y < r.bottom;
}


/**
* Create ortho matrix */
CORE_API LcMatrix4 OrthoMatrix(float widthPixels, float heightPixels, float nearPlane, float farPlane);
/**
* Create ortho matrix */
CORE_API LcMatrix4 OrthoMatrix(LcSize viewportSize, float nearPlane, float farPlane);
/**
* Create look at matrix */
CORE_API LcMatrix4 LookAtMatrix(LcVector3 from, LcVector3 to);
/**
* Create translation matrix */
CORE_API LcMatrix4 TranslationMatrix(LcVector3 pos);
/**
* Create transform matrix */
CORE_API LcMatrix4 TransformMatrix(LcVector3 pos, LcVector2 scale, float rotZ = 0.0f);
/**
* Transpose matrix */
CORE_API LcMatrix4 TransposeMatrix(const LcMatrix4& mat);
/**
* Identity matrix */
CORE_API LcMatrix4 IdentityMatrix();


/**
* Keys struct */
struct CORE_API KEYS
{
	KEYS();
	//
	unsigned char& operator[](int index);
	//
	const static int numKeys = 128;
	//
	unsigned char keys[numKeys];
};


/**
* Read text file */
CORE_API std::string ReadTextFile(const char* filePath);
/**
* Read binary file */
CORE_API std::vector<char> ReadBinaryFile(const char* filePath);


/**
* Print debug string */
CORE_API void DebugMsg(const char* fmt, ...);
/**
* Print debug string */
CORE_API void DebugMsgW(const wchar_t* fmt, ...);
