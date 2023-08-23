/**
* LCUtils.h
* 28.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "LCUtils.h"


LcVector3 LcDefaults::OneVec3 = LcVector3(1.0f, 1.0f, 1.0f);
LcVector3 LcDefaults::ZeroVec3 = LcVector3(0.0f, 0.0f, 0.0f);

#ifdef _WINDOWS
LcVector4 LcDefaults::OneVec4 = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
LcVector4 LcDefaults::ZeroVec4 = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
#endif


LcVector4 ToV(const LcColor4& v)
{
#ifdef _WINDOWS
	return DirectX::XMVectorSet(v.x, v.y, v.z, v.w);
#else
	return LcColor4{};
#endif
}

LcColor4 ToC(const LcVector4& v)
{
#ifdef _WINDOWS
	return LcColor4{ DirectX::XMVectorGetX(v), DirectX::XMVectorGetY(v), DirectX::XMVectorGetZ(v), DirectX::XMVectorGetW(v) };
#else
	return LcColor4{};
#endif
}

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

LcMatrix4 OrthoMatrix(float widthPixels, float heightPixels, float nearPlane, float farPlane)
{
#ifdef _WINDOWS
	return DirectX::XMMatrixOrthographicLH(widthPixels, heightPixels, nearPlane, farPlane);
#else
	return LcMatrix4{};
#endif
}

LcMatrix4 OrthoMatrix(LcSize vp, float nearPlane, float farPlane)
{
	return OrthoMatrix((float)vp.x, (float)vp.y, nearPlane, farPlane);
}

LcMatrix4 LookAtMatrix(LcVector3 from, LcVector3 to)
{
#ifdef _WINDOWS
	return DirectX::XMMatrixLookAtLH(
		DirectX::XMVectorSet(from.x, from.y, from.z, 0.0f),
		DirectX::XMVectorSet(to.x, to.y, to.z, 0.0f),
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
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

LcMatrix4 TransformMatrix(LcVector3 pos, LcVector2 scale, float rotZ)
{
#ifdef _WINDOWS
	return DirectX::XMMatrixTransformation(
		LcDefaults::ZeroVec4, LcDefaults::ZeroVec4,
		DirectX::XMVectorSet(scale.x, scale.y, 0.0f, 1.0f),
		LcDefaults::ZeroVec4,
		DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f), rotZ),
		DirectX::XMVectorSet(pos.x, pos.y, pos.z, 1.0f));
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


KEYS::KEYS()
{
	memset(keys, 0, sizeof(keys));
}

unsigned char& KEYS::operator[](int index)
{
	if (index < 0 || index >= numKeys) throw std::exception("KEYS::operator[]: Invalid index");

	return keys[index];
}


std::string ReadTextFile(const char* filePath)
{
	using namespace std::filesystem;

	path path;
	path.assign(filePath);
	std::ifstream stream(path, std::ios::in | std::ios::binary);

	const auto sz = file_size(path);
	std::string result(sz, '\0');
	stream.read(result.data(), sz);

	return result;
}

std::vector<char> ReadBinaryFile(const char* filePath)
{
	using namespace std::filesystem;

	path path;
	path.assign(filePath);
	std::ifstream stream(path, std::ios::in | std::ios::binary);

	const auto sz = file_size(path);
	std::vector<char> result(sz);
	stream.read(result.data(), sz);

	return result;
}

#ifdef _WINDOWS

#include <windows.h>
#include <stdio.h>

void DebugMsg(const char* fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	char dbg_out[4096];
	vsprintf_s(dbg_out, fmt, argp);
	va_end(argp);
	OutputDebugStringA(dbg_out);
}

void DebugMsgW(const wchar_t* fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	wchar_t dbg_out[4096];
	vswprintf_s(dbg_out, fmt, argp);
	va_end(argp);
	OutputDebugStringW(dbg_out);
}

#else

void DebugMsg(const char* fmt, ...) {}
void DebugMsgW(const wchar_t* fmt, ...) {}

#endif
