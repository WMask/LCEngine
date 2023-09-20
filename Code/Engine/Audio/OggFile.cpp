/**
* OggFile.cpp
* 20.09.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "Audio/OggFile.h"
#include "Core/LCException.h"
#include "Core/LCUtils.h"

#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"


struct RaiiFILE
{
    RaiiFILE() : file(nullptr) {}
    //
    ~RaiiFILE() { if (file) fclose(file); }
    //
    FILE* file;
};

void LcOggFile::Load(const char* filePath)
{
    LC_TRY

    RaiiFILE rf;
    fopen_s(&rf.file, filePath, "rb");
    if (!rf.file)
    {
        throw std::exception("LcRiffFile(): Cannot open ogg file");
    }

    OggVorbis_File vf;
    if (ov_open_callbacks(rf.file, &vf, NULL, 0, OV_CALLBACKS_NOCLOSE) < 0)
    {
        throw std::exception("LcRiffFile(): Cannot read ogg data");
    }

    vorbis_info* vi = ov_info(&vf, -1);
    memset(&fmt, 0, sizeof(fmt));
    fmt.Format.cbSize = sizeof(WAVEFORMATEX);
    fmt.Format.nChannels = vi->channels;
    fmt.Format.wBitsPerSample = 16;
    fmt.Format.nSamplesPerSec = vi->rate;
    fmt.Format.nAvgBytesPerSec = fmt.Format.nSamplesPerSec * fmt.Format.nChannels * 2;
    fmt.Format.nBlockAlign = 2 * fmt.Format.nChannels;
    fmt.Format.wFormatTag = 1;

    audioData.reserve((int)ov_pcm_total(&vf, -1));

    bool eof = false;
    int curSection = 0;
    char pcmout[4096];

    while (!eof)
    {
        int bytesRead = ov_read(&vf, pcmout, sizeof(pcmout), 0, 2, 1, &curSection);
        if (bytesRead == 0)
        {
            eof = true;
        }
        else if (bytesRead < 0)
        {
            if (bytesRead == OV_EBADLINK)
            {
                throw std::exception("LcRiffFile(): Ogg data error");
            }
        }
        else
        {
            const BYTE* newDataPtr = (BYTE*)pcmout;
            audioData.insert(audioData.end(), newDataPtr, newDataPtr + bytesRead);
        }
    }

    ov_clear(&vf);

    LC_CATCH{ LC_THROW_EX("LcOggFile(): '", filePath, "'") }
}
