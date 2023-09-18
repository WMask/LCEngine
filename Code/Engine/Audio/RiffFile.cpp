/**
* XAudio2System.cpp
* 18.09.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "Audio/RiffFile.h"
#include "Core/LCException.h"
#include "Core/LCUtils.h"


#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT  ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'


bool FindChunk(const LcBytes& fileData, DWORD chunkName, DWORD& chunkOffset, DWORD& chunkDataSize)
{
    DWORD chunkType;
    DWORD offset = 0;

    while ((offset + sizeof(DWORD) * 2) < fileData.size())
    {
        memcpy(&chunkType, &fileData[offset], sizeof(DWORD));
        memcpy(&chunkDataSize, &fileData[offset + sizeof(DWORD)], sizeof(DWORD));

        if (chunkType == chunkName)
        {
            chunkOffset = offset;
            return true;
        }

        offset += (chunkType == fourccRIFF) ? (sizeof(DWORD) * 3) : (sizeof(DWORD) * 2 + chunkDataSize);
    }

    return false;
}

LcRiffFile::LcRiffFile(const char* filePath)
{
    LC_TRY

    LcBytes fileData = ReadBinaryFile(filePath);
    if (fileData.size() < 64)
    {
        throw std::exception("LcRiffFile(): Invalid file type");
    }

    DWORD chunkOffset = 0, chunkDataSize = 0;
    if (!FindChunk(fileData, fourccRIFF, chunkOffset, chunkDataSize))
    {
        throw std::exception("LcRiffFile(): Cannot find RIFF chunk");
    }

    // read file type
    DWORD fileType = 0;
    memcpy(&fileType, &fileData[chunkOffset + sizeof(DWORD) * 2], sizeof(DWORD));
    if (fileType != fourccWAVE)
    {
        throw std::exception("LcRiffFile(): Unknown file type");
    }

    if (!FindChunk(fileData, fourccFMT, chunkOffset, chunkDataSize))
    {
        throw std::exception("LcRiffFile(): Cannot read audio format");
    }

    // read audio format
    memset(&fmt, 0, sizeof(WAVEFORMATEXTENSIBLE));
    memcpy(&fmt, &fileData[chunkOffset + sizeof(DWORD) * 2], chunkDataSize);

    if (!FindChunk(fileData, fourccDATA, chunkOffset, chunkDataSize))
    {
        throw std::exception("LcRiffFile(): Cannot find audio data chunk");
    }

    // read audio data
    audioData.resize(chunkDataSize);
    memcpy(&audioData[0], &fileData[chunkOffset + sizeof(DWORD) * 2], chunkDataSize);

    LC_CATCH{ LC_THROW_EX("LcRiffFile(): '", filePath, "'") }
}

LcRiffFile::~LcRiffFile()
{
}
