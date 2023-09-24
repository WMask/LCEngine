/**
* LCUtils.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "LCUtils.h"
#include "LCException.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <cctype>


std::string ReadTextFile(const char* filePath)
{
	using namespace std::filesystem;

	std::string result;

	LC_TRY

	path path;
	path.assign(filePath);
	std::ifstream stream(path, std::ios::in | std::ios::binary);

	const auto sz = file_size(path);
	result = std::string(sz, '\0');
	stream.read(result.data(), sz);

	LC_CATCH{ LC_THROW_EX("ReadTextFile('", filePath, "')"); }

	return result;
}

LcBytes ReadBinaryFile(const char* filePath)
{
	using namespace std::filesystem;

	LcBytes result;

	LC_TRY

	path path;
	path.assign(filePath);
	std::ifstream stream(path, std::ios::in | std::ios::binary);

	const auto sz = file_size(path);
	result = LcBytes(sz);
	stream.read((char*)result.data(), sz);

	LC_CATCH{ LC_THROW_EX("ReadBinaryFile('", filePath, "')"); }

	return result;
}

void WriteTextFile(const char* filePath, const std::string& text)
{
	using namespace std::filesystem;

	LC_TRY

	path path;
	path.assign(filePath);
	std::ofstream stream(path, std::ios::out);

	stream.write(text.c_str(), text.length());

	LC_CATCH{ LC_THROW_EX("WriteTextFile('", filePath, "')"); }
}

std::string ToLower(const char* str)
{
	std::string src(str);
	std::string dst;
	dst.resize(src.size());

	std::transform(src.begin(), src.end(), dst.begin(), ::tolower);

	return dst;
}

std::string ToUpper(const char* str)
{
	std::string src(str);
	std::string dst;
	dst.resize(src.size());

	std::transform(src.begin(), src.end(), dst.begin(), ::toupper);

	return dst;
}

std::string ToString(int value)
{
	std::stringstream ss;
	ss << value;
	return ss.str();
}

std::string ToString(float value)
{
	std::stringstream ss;
	ss << value;
	return ss.str();
}

std::wstring ToStringW(int value)
{
	std::wstringstream ss;
	ss << value;
	return ss.str();
}

std::wstring ToStringW(float value)
{
	std::wstringstream ss;
	ss << value;
	return ss.str();
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

void ShowMessageModal(const char* message, const char* title)
{
	MessageBoxA(NULL, message, title, MB_OK | MB_SERVICE_NOTIFICATION);
}

#else

void DebugMsg(const char* fmt, ...) {}
void DebugMsgW(const wchar_t* fmt, ...) {}

#endif
