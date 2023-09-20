/**
* XAudio2Sound.cpp
* 19.09.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "Audio/XAudio2/XAudio2Sound.h"
#include "Core/LCException.h"
#include "Core/LCUtils.h"
#include "Audio/libvorbis/include/vorbis/vorbisfile.h"


LcXAudio2Sound::LcXAudio2Sound()
	: voice(nullptr)
	, streamed(false)
	, currentBuffer(0)
	, riffBuffers{}
	, oggBuffers{}
	, xBuffers{}
{
}

LcXAudio2Sound::~LcXAudio2Sound()
{
	if (voice)
	{
		voice->Stop(0);
		voice->DestroyVoice();
		voice = nullptr;
	}
}

void LcXAudio2Sound::Load(const char* filePath, IXAudio2* audio)
{
	auto path = ToLower(filePath);
	streamed = (path.find("ogg") != std::string::npos);

	auto& xBuffer = xBuffers[currentBuffer];

	if (streamed)
	{
		auto& oggBuffer = oggBuffers[currentBuffer];
		oggBuffer.Load(filePath);

		xBuffer.AudioBytes = oggBuffer.getDataSize();
		xBuffer.pAudioData = oggBuffer.getAudioData();
		xBuffer.Flags = XAUDIO2_END_OF_STREAM;

		if (FAILED(audio->CreateSourceVoice(&voice, oggBuffer.getFormat())))
		{
			throw std::exception("LcXAudio2Sound::Load(): Cannot create voice");
		}
	}
	else
	{
		auto& riffBuffer = riffBuffers[currentBuffer];
		riffBuffer.Load(filePath);

		xBuffer.AudioBytes = riffBuffer.getDataSize();
		xBuffer.pAudioData = riffBuffer.getAudioData();
		xBuffer.Flags = XAUDIO2_END_OF_STREAM;

		if (FAILED(audio->CreateSourceVoice(&voice, riffBuffer.getFormat())))
		{
			throw std::exception("LcXAudio2Sound::Load(): Cannot create voice");
		}
	}
}

void LcXAudio2Sound::Update(float deltaSeconds, const LcAppContext& context)
{
}

void LcXAudio2Sound::Play(bool looped)
{
	if (!voice) return;

	if (streamed)
	{
		voice->Stop(0);
		voice->FlushSourceBuffers();

		auto& xBuffer = xBuffers[currentBuffer];
		if (FAILED(voice->SubmitSourceBuffer(&xBuffer)))
		{
			throw std::exception("LcXAudio2Sound::Play(): Cannot set buffer");
		}

		voice->Start(0);
	}
	else
	{
		voice->Stop(0);
		voice->FlushSourceBuffers();

		auto& xBuffer = xBuffers[currentBuffer];
		if (FAILED(voice->SubmitSourceBuffer(&xBuffer)))
		{
			throw std::exception("LcXAudio2Sound::Play(): Cannot set buffer");
		}

		voice->Start(0);
	}
}

void LcXAudio2Sound::Stop()
{
}

void LcXAudio2Sound::Pause()
{
}

void LcXAudio2Sound::Resume()
{
}

bool LcXAudio2Sound::IsPlaying()
{
	return false;
}

bool LcXAudio2Sound::IsPaused()
{
	return false;
}
