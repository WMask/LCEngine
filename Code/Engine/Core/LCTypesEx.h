/**
* LCTypesEx.h
* 24.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "LCTypes.h"

#ifdef _WINDOWS
# include <windows.h>
# include <directxmath.h>
# include <dcommon.h>
#endif


/** Geometry */
#ifdef _WINDOWS
typedef DirectX::XMINT2		LcPoint;
typedef DirectX::XMINT2		LcSize;
typedef DirectX::XMFLOAT2	LcSizef;
typedef DirectX::XMFLOAT2	LcVector2;
typedef DirectX::XMFLOAT3	LcVector3;
typedef DirectX::XMFLOAT4	LcColor4;
typedef DirectX::XMVECTOR	LcVector4;
typedef DirectX::XMMATRIX	LcMatrix4;
typedef RECT				LcRect;
typedef D2D_RECT_F			LcRectf;

inline LcVector2 operator+(const LcVector2& a, const LcVector2& b)
{
	return LcVector2{ a.x + b.x, a.y + b.y };
}

inline LcVector3 operator+(const LcVector3& a, const LcVector3& b)
{
	return LcVector3{ a.x + b.x, a.y + b.y, a.z + b.z };
}

inline LcVector4 operator+(const LcVector4& a, const LcVector4& b)
{
#ifdef _WINDOWS
	return DirectX::XMVectorAdd(a, b);
#else
	return LcVector4{};
#endif
}

#endif


/** Defaults */
namespace LcDefaults
{
	extern CORE_API LcVector2 OneVec2;
	extern CORE_API LcVector2 ZeroVec2;
	extern CORE_API LcVector3 OneVec3;
	extern CORE_API LcVector3 ZeroVec3;
	extern CORE_API LcVector4 OneVec4;
	extern CORE_API LcVector4 ZeroVec4;
	extern CORE_API LcColor4 White;
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
