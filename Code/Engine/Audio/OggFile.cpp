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

#define LC_OGG_CHUNK_SIZE (1024 * 4)
#define LC_OGG_BUF_SIZE   (1024 * 16)

using OggFile = LcOggFile::LcOggFileReader;


size_t LcReadOgg(void* dst, size_t size1, size_t size2, void* fh)
{
    OggFile* of = reinterpret_cast<OggFile*>(fh);
    size_t len = size1 * size2;
    if (of->curPtr + len > of->dataPtr + of->dataSize)
    {
        len = of->dataPtr + of->dataSize - of->curPtr;
    }
    memcpy(dst, of->curPtr, len);
    of->curPtr += len;
    return len;
}

int LcSeekOgg(void* fh, ogg_int64_t to, int type)
{
    OggFile* of = reinterpret_cast<OggFile*>(fh);

    switch (type) {
    case SEEK_CUR:
        of->curPtr += to;
        break;
    case SEEK_END:
        of->curPtr = of->dataPtr + of->dataSize - to;
        break;
    case SEEK_SET:
        of->curPtr = of->dataPtr + to;
        break;
    default:
        return -1;
    }
    if (of->curPtr < of->dataPtr) {
        of->curPtr = of->dataPtr;
        return -1;
    }
    if (of->curPtr > of->dataPtr + of->dataSize) {
        of->curPtr = of->dataPtr + of->dataSize;
        return -1;
    }
    return 0;
}

int LcCloseOgg(void* fh)
{
    return 0;
}

long LcTellOgg(void* fh)
{
    OggFile* of = reinterpret_cast<OggFile*>(fh);
    return long(of->curPtr - of->dataPtr);
}

static ov_callbacks callbacks{ LcReadOgg, LcSeekOgg, LcCloseOgg, LcTellOgg };

bool FillBuffer(LcBytes& buffer, OggVorbis_File& vf, bool& eof)
{
    int curSection = 0, fullSize = 0;
    char tmpBuf[LC_OGG_CHUNK_SIZE];

    buffer.clear();
    buffer.reserve(LC_OGG_BUF_SIZE);

    while (!eof)
    {
        int bytesRead = ov_read(&vf, tmpBuf, sizeof(tmpBuf), 0, 2, 1, &curSection);
        fullSize += bytesRead;
        if (bytesRead == 0)
        {
            eof = true;
        }
        else if (bytesRead < 0)
        {
            if (bytesRead == OV_EBADLINK)
            {
                throw std::exception("LcOggFile(): Data error");
            }
        }
        else
        {
            const BYTE* newDataPtr = (BYTE*)tmpBuf;
            buffer.insert(buffer.end(), newDataPtr, newDataPtr + bytesRead);
            if (buffer.size() > LC_OGG_BUF_SIZE)
            {
                break;
            }
        }
    }

    return fullSize != 0;
}


LcOggFile::LcOggFile() : reader{}, fmt{}, file{}, curBufId(0), streamEOF(false)
{
}

LcOggFile::~LcOggFile()
{
    if (file)
    {
        ov_clear(file.get());
        file.reset();
    }
}

void LcOggFile::Load(const char* filePath)
{
    LC_TRY

    compressedData = ReadBinaryFile(filePath);
    if (compressedData.size() < 64)
    {
        throw std::exception("LcOggFile(): Invalid file type");
    }

    reader.curPtr = reader.dataPtr = (char*)&compressedData[0];
    reader.dataSize = compressedData.size();

    std::shared_ptr<OggVorbis_File> newFile(std::make_shared<OggVorbis_File>());

    if (ov_open_callbacks((void*)&reader, newFile.get(), NULL, -1, callbacks) < 0)
    {
        throw std::exception("LcOggFile(): Cannot read ogg data");
    }

    vorbis_info* vi = ov_info(newFile.get(), -1);
    memset(&fmt, 0, sizeof(fmt));
    fmt.Format.cbSize = sizeof(WAVEFORMATEX);
    fmt.Format.nChannels = vi->channels;
    fmt.Format.wBitsPerSample = 16;
    fmt.Format.nSamplesPerSec = vi->rate;
    fmt.Format.nAvgBytesPerSec = fmt.Format.nSamplesPerSec * fmt.Format.nChannels * 2;
    fmt.Format.nBlockAlign = 2 * fmt.Format.nChannels;
    fmt.Format.wFormatTag = 1;

    FillBuffer(buffers[0], *newFile, streamEOF);

    file = newFile;

    LC_CATCH{ LC_THROW_EX("LcOggFile(): '", filePath, "'") }
}

bool LcOggFile::RequestNextBuffer()
{
    if (streamEOF) return false;

    curBufId = (curBufId == 0) ? 1 : 0;
    LcBytes* curBuf = &buffers[curBufId];

    return FillBuffer(*curBuf, *file, streamEOF);
}

void LcOggFile::Stop()
{
    if (file)
    {
        if (!compressedData.empty())
        {
            reader.curPtr = (char*)&compressedData[0];
        }

        ov_clear(file.get());
        file.reset(new OggVorbis_File());
        if (ov_open_callbacks((void*)&reader, file.get(), NULL, -1, callbacks) < 0)
        {
            throw std::exception("LcOggFile::Stop(): Cannot read ogg data");
        }

        streamEOF = false;
        curBufId = 0;
        buffers[1].clear();
        FillBuffer(buffers[0], *file, streamEOF);
    }
}
