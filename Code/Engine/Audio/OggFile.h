/**
* OggFile.h
* 20.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/LcTypes.h"

#include <mmreg.h>

#pragma warning(disable : 4251)


/**
* Ogg file */
class AUDIO_API LcOggFile
{
public:
	struct LcOggFileReader
	{
		char* curPtr;
		char* dataPtr;
		size_t dataSize;
	};


public:
	LcOggFile();
	//
	~LcOggFile();
	//
	void Load(const char* filePath);
	//
	bool RequestNextBuffer();
	// reset reader to file start position
	void Stop();
	//
	const BYTE* getAudioData() const { return (buffers[curBufId].size() == 0) ? nullptr : &buffers[curBufId][0]; }
	//
	DWORD getDataSize() const { return (DWORD)buffers[curBufId].size(); }
	//
	const WAVEFORMATEX* getFormat() const { return (WAVEFORMATEX*)&fmt; }
	//
	bool IsEOF() const { return streamEOF; }


protected:
	//
	std::shared_ptr<struct OggVorbis_File> file;
	//
	LcOggFileReader reader;
	//
	WAVEFORMATEXTENSIBLE fmt;
	//
	LcBytes buffers[2];
	//
	LcBytes compressedData;
	//
	int curBufId;
	//
	bool streamEOF;

};
