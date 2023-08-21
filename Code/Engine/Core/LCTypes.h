/**
* LCTypes.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#ifdef _WINDOWS
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
