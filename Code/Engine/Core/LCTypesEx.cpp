/**
* LCTypesEx.cpp
* 24.08.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "LCTypesEx.h"


LcVector2 LcDefaults::OneVec2 = LcVector2{ 1.0f, 1.0f };
LcVector2 LcDefaults::ZeroVec2 = LcVector2{ 0.0f, 0.0f };
LcVector3 LcDefaults::OneVec3 = LcVector3{ 1.0f, 1.0f, 1.0f };
LcVector3 LcDefaults::ZeroVec3 = LcVector3{ 0.0f, 0.0f, 0.0f };
LcVector4 LcDefaults::OneVec4 = LcVector4{ 1.0f, 1.0f, 1.0f, 1.0f };
LcVector4 LcDefaults::ZeroVec4 = LcVector4{ 0.0f, 0.0f, 0.0f, 0.0f };
LcColor4 LcDefaults::Invisible = LcColor4{ 0.0f, 0.0f, 0.0f, 0.0f };
LcColor4 LcDefaults::White4 = LcColor4{ 1.0f, 1.0f, 1.0f, 1.0f };
LcColor3 LcDefaults::White3 = LcColor3{ 1.0f, 1.0f, 1.0f };
LcColor4 LcDefaults::Black4 = LcColor4{ 0.0f, 0.0f, 0.0f, 1.0f };
LcColor3 LcDefaults::Black3 = LcColor3{ 0.0f, 0.0f, 0.0f };
LcSizef LcDefaults::ZeroSize = LcSizef{ 0.0f, 0.0f };

#ifdef _WINDOWS
DirectX::XMVECTOR LcDefaults::OneXVec4 = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
DirectX::XMVECTOR LcDefaults::ZeroXVec4 = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
#endif


LcRectf ToF(const LcRect& rect)
{
#ifdef _WINDOWS
	return LcRectf{ (float)rect.left, (float)rect.top, (float)rect.right, (float)rect.bottom };
#else
	return LcRectf{};
#endif
}

LcRectf ToF(const LcVector2& leftTop, const LcVector2& rightBottom)
{
#ifdef _WINDOWS
	return LcRectf{ (float)leftTop.x, (float)leftTop.y, (float)rightBottom.x, (float)rightBottom.y };
#else
	return LcRectf{};
#endif
}

LcRect ToI(const LcRectf& rect)
{
#ifdef _WINDOWS
	return LcRect{ (int)rect.left, (int)rect.top, (int)rect.right, (int)rect.bottom };
#else
	return LcRect{};
#endif
}

LcMatrix4 OrthoMatrix(float widthPixels, float heightPixels, float nearPlane, float farPlane, bool flipY)
{
#ifdef _WINDOWS
	auto matrix = DirectX::XMMatrixOrthographicLH(widthPixels, heightPixels, nearPlane, farPlane);
	if (flipY) matrix = DirectX::XMMatrixMultiply(matrix, DirectX::XMMatrixScaling(1.0f, -1.0f, 1.0f));
	return TransposeMatrix(matrix);
#else
	return LcMatrix4{};
#endif
}

LcMatrix4 OrthoMatrix(LcSize vp, float nearPlane, float farPlane, bool flipY)
{
	return OrthoMatrix((float)vp.x, (float)vp.y, nearPlane, farPlane, flipY);
}

LcMatrix4 LookAtMatrix(LcVector3 from, LcVector3 to)
{
#ifdef _WINDOWS
	auto matrix = DirectX::XMMatrixLookAtLH(
		DirectX::XMVectorSet(from.x, from.y, from.z, 0.0f),
		DirectX::XMVectorSet(to.x, to.y, to.z, 0.0f),
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	return TransposeMatrix(matrix);
#else
	return LcMatrix4{};
#endif
}

LcMatrix4 TranslationMatrix(LcVector3 pos)
{
#ifdef _WINDOWS
	return DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
#else
	return LcMatrix4{};
#endif
}

LcMatrix4 TransformMatrix(LcVector3 pos, LcVector2 scale, float rotZ, bool flipY)
{
#ifdef _WINDOWS
	auto matrix = DirectX::XMMatrixTransformation(
		LcDefaults::ZeroXVec4, LcDefaults::ZeroXVec4,
		DirectX::XMVectorSet(scale.x, flipY ? -scale.y : scale.y, 0.0f, 1.0f),
		LcDefaults::ZeroXVec4,
		DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f), rotZ),
		DirectX::XMVectorSet(pos.x, pos.y, pos.z, 1.0f));
	return TransposeMatrix(matrix);
#else
	return LcMatrix4{};
#endif
}

LcMatrix4 TransposeMatrix(const LcMatrix4& mat)
{
#ifdef _WINDOWS
	return DirectX::XMMatrixTranspose(mat);
#else
	return LcMatrix4{};
#endif
}

LcMatrix4 IdentityMatrix()
{
#ifdef _WINDOWS
	return DirectX::XMMatrixIdentity();
#else
	return LcMatrix4{};
#endif
}
