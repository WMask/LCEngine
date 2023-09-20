/**
* OggFile.h
* 20.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/LcTypes.h"

#include <string>
#include <mmreg.h>

#pragma warning(disable : 4251)


/**
* Ogg file */
class AUDIO_API LcOggFile
{
public:
	LcOggFile() : fmt{} {}
	//
	~LcOggFile() {}
	//
	void Load(const char* filePath);
	//
	const BYTE* getAudioData() const { return (audioData.size() == 0) ? nullptr : &audioData[0]; }
	//
	DWORD getDataSize() const { return (DWORD)audioData.size(); }
	//
	const WAVEFORMATEX* getFormat() const { return (WAVEFORMATEX*)&fmt; }


protected:
	WAVEFORMATEXTENSIBLE fmt;
	//
	LcBytes audioData;

};
