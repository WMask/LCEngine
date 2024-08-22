/**
* LCUtils.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "LCUtils.h"
#include "LCException.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>

#ifdef _WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#endif

#include "Core/libpng/Include/png.h"
static const int MAX_PNG_SIZE = 4096;

struct FileRAII : public LcUncopyable
{
	FileRAII(const char* filePath) : file(nullptr)
	{
#ifdef _WINDOWS
		fopen_s(&file, filePath, "rb");
#else
		file = fopen(filePath, "rb");
#endif
	}
	~FileRAII() { if (file) fclose(file); }
	operator bool() const { return file != nullptr; }
	operator FILE* () const { return file; }
	FILE* file;
};


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

void ReadPngFile(const char* filePath, int* outWidth, int* outHeight, int* outBPP, int* outRowBytes, void* outData)
{
	struct PngRAII
	{
		PngRAII(png_structp& in_png_ptr, png_infop& in_info_ptr, png_infop& in_end_info)
			: png_ptr(in_png_ptr)
			, info_ptr(in_info_ptr)
			, end_info(in_end_info)
		{}
		~PngRAII() { png_destroy_read_struct(&png_ptr, &info_ptr, &end_info); }
		png_structp& png_ptr;
		png_infop& info_ptr;
		png_infop& end_info;
	};

	LC_TRY

	FileRAII fp(filePath);
	if (!fp)
	{
		throw LcException("Failed to read file");
	}

	const int sig_bytes = 8;
	png_byte png_header[sig_bytes];
	fread(png_header, 1, sig_bytes, fp);
	bool is_png = !png_sig_cmp(png_header, 0, sig_bytes);
	if (!is_png)
	{
		throw LcException("Invalid png header");
	}

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if (!png_ptr)
	{
		throw LcException("Failed to create read struct");
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, 0, 0);
		throw LcException("Failed to create info struct");
	}

	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);
		throw LcException("Failed to create end struct");
	}

	PngRAII png(png_ptr, info_ptr, end_info);

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, sig_bytes);
	png_read_info(png_ptr, info_ptr);

	int width = static_cast<int>(png_get_image_width(png_ptr, info_ptr));
	int height = static_cast<int>(png_get_image_height(png_ptr, info_ptr));
	if (!InRange(width, 0, MAX_PNG_SIZE) ||
		!InRange(height, 0, MAX_PNG_SIZE))
	{
		throw LcException("Invalid image size");
	}

	auto row_bytes = png_get_rowbytes(png_ptr, info_ptr);
	if (row_bytes == 0)
	{
		throw LcException("Invalid row bytes");
	}

	auto color_type = png_get_color_type(png_ptr, info_ptr);
	if (color_type != PNG_COLOR_TYPE_RGB &&
		color_type != PNG_COLOR_TYPE_RGBA)
	{
		throw LcException("Invalid color type");
	}

	auto bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	if (bit_depth != 8)
	{
		throw LcException("Invalid bit depth");
	}

	if (outHeight) *outHeight = height;
	if (outWidth) *outWidth = width;
	if (outRowBytes) *outRowBytes = static_cast<int>(width * 4);
	if (outBPP) *outBPP = 4; // Always 4 bytes per pixel
	if (outData)
	{
		std::unique_ptr<png_byte[]> tmp_row;
		if (color_type == PNG_COLOR_TYPE_RGB)
		{
			// Need temp row for BPP == 3
			tmp_row.reset(new png_byte[row_bytes]);
		}

		png_byte* rows = reinterpret_cast<png_byte*>(outData);
		for (int i = 0; i < height; i++)
		{
			if (color_type == PNG_COLOR_TYPE_RGBA)
			{
				// Read 4 bytes per pixel
				png_read_row(png_ptr, rows, 0);
			}
			else
			{
				// Read 3 bytes per pixel
				png_byte* row_pixels = tmp_row.get();
				png_read_row(png_ptr, row_pixels, 0);

				// Convert to 4 bytes
				for (int j = 0; j < width; j++)
				{
					memcpy(&rows[j * 4], &row_pixels[j * 3], 3);
					rows[j * 4 + 3] = 255;
				}
			}

			rows += width * 4;
		}
	}

	LC_CATCH{ LC_THROW_EX("ReadPngFile('", filePath, "')"); }
}

std::string ToUtf8(const std::wstring& str)
{
#ifdef _WINDOWS
	int requiredSize = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), (int)str.length(), NULL, 0, NULL, NULL);
	if (requiredSize <= 0) throw std::exception("ToUtf8(): Convert error");

	std::string mbChars(requiredSize, ' ');
	int result = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), (int)str.length(), &mbChars[0], (int)mbChars.length(), NULL, NULL);
	if (result != requiredSize) throw std::exception("ToUtf8(): Cannot convert");

	return mbChars;
#elif __APPLE__
    return std::string();
#endif
}

std::wstring FromUtf8(const std::string& str)
{
#ifdef _WINDOWS
	int requiredSize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), NULL, 0);
	if (requiredSize <= 0) throw std::exception("FromUtf8(): Convert error");

	std::wstring wideChars(requiredSize, ' ');
	int result = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), &wideChars[0], (int)wideChars.length());
	if (result != requiredSize) throw std::exception("FromUtf8(): Cannot convert");

	return wideChars;
#elif __APPLE__
    return std::wstring();
#endif
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

int RandHelper(int randMin, int randMax)
{
	int diff = randMax - randMin + 1;
	return randMin + rand() / (RAND_MAX / diff);
}

float RandHelper()
{
	return (float)rand() / (float)RAND_MAX;
}

#ifdef _WINDOWS

#include <windows.h>
#include <stdio.h>
#include <iostream>

void DebugMsg(const char* fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	char dbg_out[4096];
	vsprintf_s(dbg_out, fmt, argp);
	va_end(argp);
	OutputDebugStringA(dbg_out);
	std::cout << dbg_out;
}

void DebugMsgW(const wchar_t* fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	wchar_t dbg_out[4096];
	vswprintf_s(dbg_out, fmt, argp);
	va_end(argp);
	OutputDebugStringW(dbg_out);
	std::cout << dbg_out;
}

void ShowMessageModal(const char* message, const char* title)
{
	MessageBoxA(NULL, message, title, MB_OK | MB_SERVICE_NOTIFICATION);
}

#elif __APPLE__

#include <stdio.h>
#include <iostream>

void DebugMsg(const char* fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    char dbg_out[4096];
    vsnprintf(dbg_out, 4096, fmt, argp);
    va_end(argp);
    std::cout << dbg_out << std::endl;
}

#else

void DebugMsg(const char* fmt, ...) {}
void DebugMsgW(const wchar_t* fmt, ...) {}

#endif
