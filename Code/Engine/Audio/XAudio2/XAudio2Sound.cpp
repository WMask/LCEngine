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
	: streamedEndHandler{}
	, voice(nullptr)
	, streamed(false)
	, playing(false)
	, paused(false)
	, xBuffer{}
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
	streamed = (path.find(".ogg") == path.length() - 4);

	if (streamed)
	{
		oggBuffer.Load(filePath);

		if (FAILED(audio->CreateSourceVoice(&voice, oggBuffer.getFormat())))
		{
			throw std::exception("LcXAudio2Sound::Load(): Cannot create voice");
		}

		InitStreamedBuffers();
	}
	else
	{
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

void LcXAudio2Sound::InitStreamedBuffers()
{
	// need at least 2 buffers
	xBuffer.AudioBytes = oggBuffer.getDataSize();
	xBuffer.pAudioData = oggBuffer.getAudioData();
	if (FAILED(voice->SubmitSourceBuffer(&xBuffer)))
	{
		throw std::exception("LcXAudio2Sound::InitStreamedBuffers(): Cannot set buffer");
	}

	oggBuffer.RequestNextBuffer();
	xBuffer.AudioBytes = oggBuffer.getDataSize();
	xBuffer.pAudioData = oggBuffer.getAudioData();
	if (FAILED(voice->SubmitSourceBuffer(&xBuffer)))
	{
		throw std::exception("LcXAudio2Sound::InitStreamedBuffers(): Cannot set buffer");
	}
}

void LcXAudio2Sound::Update(float deltaSeconds, const LcAppContext& context)
{
	if (!voice) return;

	if (playing)
	{
		XAUDIO2_VOICE_STATE state{};
		voice->GetState(&state);

		if (streamed)
		{
			if (state.BuffersQueued < 2)
			{
				// need at least 2 buffers
				if (oggBuffer.RequestNextBuffer())
				{
					xBuffer.AudioBytes = oggBuffer.getDataSize();
					xBuffer.pAudioData = oggBuffer.getAudioData();
					xBuffer.Flags = oggBuffer.IsEOF() ? XAUDIO2_END_OF_STREAM : 0;

					if (FAILED(voice->SubmitSourceBuffer(&xBuffer)))
					{
						throw std::exception("LcXAudio2Sound::Update(): Cannot set buffer");
					}
				}
			}
		}

		if (state.BuffersQueued == 0 && oggBuffer.IsEOF())
		{
			voice->Stop(0);
			voice->FlushSourceBuffers();

			playing = false;
			paused = false;

			if (streamed)
			{
				// reset to start
				oggBuffer.Stop();
				InitStreamedBuffers();

				if (streamedEndHandler) streamedEndHandler(*this);
			}
		}
	}
}

void LcXAudio2Sound::Play()
{
	if (playing || !voice) return;

	if (streamed)
	{
		voice->Start(0);
	}
	else
	{
		if (!paused)
		{
			voice->Stop(0);
			voice->FlushSourceBuffers();

			if (FAILED(voice->SubmitSourceBuffer(&xBuffer)))
			{
				throw std::exception("LcXAudio2Sound::Play(): Cannot set buffer");
			}
		}

		voice->Start(0);
	}

	paused = false;
	playing = true;
}

void LcXAudio2Sound::Stop()
{
	voice->Stop(0);
	voice->FlushSourceBuffers();

	playing = false;
	paused = false;

	if (streamed)
	{
		// reset to start
		oggBuffer.Stop();
		InitStreamedBuffers();
	}
}

void LcXAudio2Sound::Pause()
{
	voice->Stop(0);

	playing = false;
	paused = true;
}

void LcXAudio2Sound::SetVolume(float volume)
{
	voice->SetVolume(volume);
}
