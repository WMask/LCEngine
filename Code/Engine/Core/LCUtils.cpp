/**
* LCUtils.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "LCUtils.h"
#include "LCException.h"


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
