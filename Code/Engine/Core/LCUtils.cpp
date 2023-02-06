/**
* LCUtils.h
* 28.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "LCUtils.h"


LcRectf ToF(const LcRect& rect)
{
	Eigen::Vector2i lt(rect.corner(Eigen::AlignedBox2i::TopLeft));
	Eigen::Vector2i rb(rect.corner(Eigen::AlignedBox2i::BottomRight));
	return LcRectf(Eigen::Vector2f((float)lt.x(), (float)lt.y()), Eigen::Vector2f((float)rb.x(), (float)rb.y()));
}

LcRect ToI(const LcRectf& rect)
{
	Eigen::Vector2f lt(rect.corner(Eigen::AlignedBox2f::TopLeft));
	Eigen::Vector2f rb(rect.corner(Eigen::AlignedBox2f::BottomRight));
	return LcRect(Eigen::Vector2i((int)lt.x(), (int)lt.y()), Eigen::Vector2i((int)rb.x(), (int)rb.y()));
}

LcMatrix4 OrthoMatrix(float left, float right, float bottom, float top, float near_plane, float far_plane)
{
	LcMatrix4 result = LcMatrix4::Zero();

	result(0, 0) = 2.0f / (right - left);
	result(1, 1) = 2.0f / (top - bottom);
	result(2, 2) = -2.0f / (far_plane - near_plane);
	result(3, 3) = 1.0f;
	result(0, 3) = -((right + left) / (right - left));
	result(1, 3) = -((top + bottom) / (top - bottom));
	result(2, 3) = -((far_plane + near_plane) / (far_plane - near_plane));

	return result;
}

LcMatrix4 OrthoMatrix(LcSize vp, float nearPlane, float farPlane)
{
	return OrthoMatrix(-vp.x() / 2.0f, vp.x() / 2.0f, vp.y() / 2.0f, -vp.y() / 2.0f, nearPlane, farPlane);
}

LcMatrix4 TranslationMatrix(LcVector3 pos)
{
	LcMatrix4 result = LcMatrix4::Identity();

	result(0, 3) = pos.x();
	result(1, 3) = pos.y();
	result(2, 3) = pos.z();

	return result;
}

LcMatrix4 TransformMatrix(LcVector3 pos, LcVector2 scale, float rotZ)
{
	LcMatrix4 result = LcMatrix4::Identity();

	result(0, 0) = scale.x() * cos(rotZ);
	result(0, 1) = scale.x() * -sin(rotZ);
	result(1, 0) = scale.y() * sin(rotZ);
	result(1, 1) = scale.y() * cos(rotZ);
	result(0, 3) = pos.x();
	result(1, 3) = pos.y();
	result(2, 3) = pos.z();

	return result;
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
