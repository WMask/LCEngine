/**
* LCTypesEx.h
* 24.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "LCTypes.h"

#include <algorithm>

#ifdef _WINDOWS
# include <directxmath.h>
#endif


/** Geometry */

#ifdef _WINDOWS

typedef DirectX::XMMATRIX	LcMatrix4;

namespace LcDefaults
{
	extern CORE_API DirectX::XMVECTOR OneXVec4;
	extern CORE_API DirectX::XMVECTOR ZeroXVec4;
}

#elif __APPLE__

struct LcMatrix4
{
    float m[16];
};

#endif

struct LcSize
{
	LcSize() : x(0), y(0) {}
	LcSize(unsigned int inX, unsigned int inY) : x(inX), y(inY) {}
	LcSize(int inX, int inY) : x((unsigned int)inX), y((unsigned int)inY) {}
	unsigned int x;
	unsigned int y;
};
typedef struct { int x; int y; } LcPoint;
typedef struct { float x; float y; } LcVector2, LcSizef;
struct LcVector3 { float x; float y; float z; };
struct LcVector4 { float x; float y; float z; float w; };
struct LcColor3 { float r; float g; float b; };
struct LcColor4 { float r; float g; float b; float a; };
struct LcRect { int left; int top; int right; int bottom; };
struct LcRectf { float left; float top; float right; float bottom; };

inline LcVector2 operator+(const LcVector2& a, const LcVector2& b) { return LcVector2{ a.x + b.x, a.y + b.y }; }
inline LcVector3 operator+(const LcVector3& a, const LcVector3& b) { return LcVector3{ a.x + b.x, a.y + b.y, a.z + b.z }; }
inline LcVector4 operator+(const LcVector4& a, const LcVector4& b) { return LcVector4{ a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; }

inline LcVector2 operator-(const LcVector2& a, const LcVector2& b) { return LcVector2{ a.x - b.x, a.y - b.y }; }
inline LcVector3 operator-(const LcVector3& a, const LcVector3& b) { return LcVector3{ a.x - b.x, a.y - b.y, a.z - b.z }; }
inline LcVector4 operator-(const LcVector4& a, const LcVector4& b) { return LcVector4{ a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; }

inline LcVector2 operator/(const LcVector2& a, float f) { return LcVector2{ a.x / f, a.y / f }; }
inline LcVector3 operator/(const LcVector3& a, float f) { return LcVector3{ a.x / f, a.y / f, a.z / f }; }
inline LcVector4 operator/(const LcVector4& a, float f) { return LcVector4{ a.x / f, a.y / f, a.z / f, a.w / f }; }

inline LcVector2 operator*(const LcVector2& a, const LcVector2& b) { return LcVector2{ a.x * b.x, a.y * b.y }; }
inline LcVector3 operator*(const LcVector3& a, const LcVector3& b) { return LcVector3{ a.x * b.x, a.y * b.y, a.z * b.z }; }

inline LcVector2 operator*(const LcVector2& a, float f) { return LcVector2{ a.x * f, a.y * f }; }
inline LcVector3 operator*(const LcVector3& a, float f) { return LcVector3{ a.x * f, a.y * f, a.z * f }; }
inline LcVector4 operator*(const LcVector4& a, float f) { return LcVector4{ a.x * f, a.y * f, a.z * f, a.w * f }; }
inline LcColor3 operator*(const LcColor3& c, float f) { return LcColor3{ c.r * f, c.g * f, c.b * f }; }
inline LcColor4 operator*(const LcColor4& c, float f) { return LcColor4{ c.r * f, c.g * f, c.b * f, c.a * f }; }

inline bool operator==(const LcSize& a, const LcSize& b) { return a.x == b.x && a.y == b.y; }
inline bool operator==(const LcVector2& a, const LcVector2& b) { return a.x == b.x && a.y == b.y; }
inline bool operator==(const LcVector3& a, const LcVector3& b) { return a.x == b.x && a.y == b.y && a.z == b.z; }
inline bool operator==(const LcVector4& a, const LcVector4& b) { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }
inline bool operator==(const LcColor3& x, const LcColor3& y) { return x.r == y.r && x.g == y.g && x.b == y.b; }
inline bool operator==(const LcColor4& x, const LcColor4& y) { return x.r == y.r && x.g == y.g && x.b == y.b && x.a == y.a; }

inline bool operator!=(const LcSize& a, const LcSize& b) { return a.x != b.x || a.y != b.y; }
inline bool operator!=(const LcVector2& a, const LcVector2& b) { return a.x != b.x || a.y != b.y; }
inline bool operator!=(const LcVector3& a, const LcVector3& b) { return a.x != b.x || a.y != b.y || a.z != b.z; }
inline bool operator!=(const LcVector4& a, const LcVector4& b) { return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w; }
inline bool operator!=(const LcColor3& x, const LcColor3& y) { return x.r != y.r || x.g != y.g || x.b != y.b; }
inline bool operator!=(const LcColor4& x, const LcColor4& y) { return x.r != y.r || x.g != y.g || x.b != y.b || x.a != y.a; }

inline bool Contains(const LcRectf& r, const LcVector2& p)
{
	return p.x >= r.left && p.x < r.right && p.y >= r.top && p.y < r.bottom;
}

inline bool Contains(const LcRect& r, const LcPoint& p)
{
	return p.x >= r.left && p.x < r.right && p.y >= r.top && p.y < r.bottom;
}


namespace LcDefaults
{
	extern CORE_API LcVector2 OneVec2;
	extern CORE_API LcVector2 ZeroVec2;
	extern CORE_API LcVector3 OneVec3;
	extern CORE_API LcVector3 ZeroVec3;
	extern CORE_API LcVector4 OneVec4;
	extern CORE_API LcVector4 ZeroVec4;
	extern CORE_API LcColor4 Invisible;
	extern CORE_API LcColor4 White4;
	extern CORE_API LcColor3 White3;
	extern CORE_API LcColor4 Black4;
	extern CORE_API LcColor3 Black3;
	extern CORE_API LcSizef ZeroSize;
};


/** Convert */

inline      LcSizef		ToF(const LcSize& size) { return LcSizef{ (float)size.x, (float)size.y }; }
inline      LcSize		ToI(const LcSizef& size) { return LcSize{ (unsigned int)size.x, (unsigned int)size.y }; }
inline      LcVector2	To2(const LcVector3& v) { return LcVector2{ v.x, v.y }; }
inline      LcVector3	To3(const LcVector2& v) { return LcVector3{ v.x, v.y, 0.0f }; }
inline	    LcColor4	To4(const LcColor3& v) { return LcColor4{ v.r, v.g, v.b, 1.0f }; }
inline	    LcVector4	To4(const LcVector2& v) { return LcVector4{ v.x, v.y, 0.0f, 0.0f }; }
inline	    LcVector4	ToV(const LcColor3& c) { return LcVector4{ c.r, c.g, c.b, 1.0f }; }
inline	    LcVector4	ToV(const LcColor4& c) { return LcVector4{ c.r, c.g, c.b, c.a }; }
CORE_API    LcRectf		ToF(const LcRect& rect);
CORE_API    LcRectf		ToF(const LcVector2& leftTop, const LcVector2& rightBottom);
CORE_API	LcRect		ToI(const LcRectf& rect);


/**
* Create ortho matrix */
CORE_API LcMatrix4 OrthoMatrix(float widthPixels, float heightPixels, float nearPlane, float farPlane, bool flipY = true);
/**
* Create ortho matrix */
CORE_API LcMatrix4 OrthoMatrix(LcSize viewportSize, float nearPlane, float farPlane, bool flipY = true);
/**
* Create look at matrix */
CORE_API LcMatrix4 LookAtMatrix(LcVector3 from, LcVector3 to);
/**
* Create translation matrix */
CORE_API LcMatrix4 TranslationMatrix(LcVector3 pos);
/**
* Create transform matrix */
CORE_API LcMatrix4 TransformMatrix(LcVector3 pos, LcVector2 scale, float rotZ = 0.0f, bool flipY = true);
/**
* Transpose matrix */
CORE_API LcMatrix4 TransposeMatrix(const LcMatrix4& mat);
/**
* Identity matrix */
CORE_API LcMatrix4 IdentityMatrix();
