/**
* LCUtils.h
* 28.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "LCUtils.h"


Eigen::Matrix4f OrthoMatrix(float left, float right, float bottom, float top, float near_plane, float far_plane)
{
	Eigen::Matrix4f result = Eigen::Matrix4f::Zero();

	result(0, 0) = 2.0f / (right - left);
	result(1, 1) = 2.0f / (top - bottom);
	result(2, 2) = -2.0f / (far_plane - near_plane);
	result(3, 3) = 1.0f;
	result(0, 3) = -((right + left) / (right - left));
	result(1, 3) = -((top + bottom) / (top - bottom));
	result(2, 3) = -((far_plane + near_plane) / (far_plane - near_plane));

	return result;
}

Eigen::Matrix4f OrthoMatrix(LCSize vp, float nearPlane, float farPlane)
{
	return OrthoMatrix(-vp.x() / 2.0f, vp.x() / 2.0f, vp.y() / 2.0f, -vp.y() / 2.0f, nearPlane, farPlane);
}

Eigen::Matrix4f TranslationMatrix(Eigen::Vector3f pos)
{
	Eigen::Matrix4f result = Eigen::Matrix4f::Identity();

	result(0, 3) = pos.x();
	result(1, 3) = pos.y();
	result(2, 3) = pos.z();

	return result;
}

Eigen::Matrix4f TransformMatrix(Eigen::Vector3f pos, Eigen::Vector2f scale, float rotZ)
{
	Eigen::Matrix4f result = Eigen::Matrix4f::Identity();

	result(0, 0) = scale.x() * cos(rotZ);
	result(0, 1) = scale.x() * -sin(rotZ);
	result(1, 0) = scale.y() * sin(rotZ);
	result(1, 1) = scale.y() * cos(rotZ);
	result(0, 3) = pos.x();
	result(1, 3) = pos.y();
	result(2, 3) = pos.z();

	return result;
}

std::string ReadTextFile(const std::string& filePath)
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
