/**
* XAudio2System.cpp
* 18.09.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "Audio/XAudio2/XAudio2System.h"
#include "Audio/RiffFile.h"
#include "Core/LCException.h"


class LcXAudio2Sound : public ISound
{
public:
	LcXAudio2Sound() : voice(nullptr), buffer{}
	{
	}
	//
	virtual ~LcXAudio2Sound() override
	{
		if (voice)
		{
			voice->Stop(0);
			voice->DestroyVoice();
			voice = nullptr;
		}
	}
	//
	void Load(const char* filePath)
	{
		data.Load(filePath);

		buffer.AudioBytes = data.getDataSize();
		buffer.pAudioData = data.getAudioData();
		buffer.Flags = XAUDIO2_END_OF_STREAM;
	}
	//
	IXAudio2SourceVoice* voice;
	//
	XAUDIO2_BUFFER buffer;
	//
	LcRiffFile data;


public: // ISound interface implementation
	//
	virtual void Update(float deltaSeconds) override {}
	//
	virtual void Play(bool looped) override
	{
		if (voice)
		{
			voice->Stop(0);
			voice->FlushSourceBuffers();

			if (FAILED(voice->SubmitSourceBuffer(&buffer)))
			{
				throw std::exception("LcXAudio2Sound::Play(): Cannot set buffer");
			}

			voice->Start(0);
		}
	}
	//
	virtual void Stop() override {}
	//
	virtual void Pause() override {}
	//
	virtual void Resume() override {}
	//
	virtual bool IsPlaying() override { return false; }
	//
	virtual bool IsPaused() override { return false; }
	//
	virtual bool IsStreamed() override { return false; }
};

class LcSoundLifetimeStrategy : public LcLifetimeStrategy<ISound>
{
public:
	LcSoundLifetimeStrategy() {}
	//
	virtual ~LcSoundLifetimeStrategy() {}
	//
	virtual std::shared_ptr<ISound> Create() override { return std::make_shared<LcXAudio2Sound>(); }
	//
	virtual void Destroy(ISound& item) override {}
};


LcXAudio2System::LcXAudio2System() : masteringVoice(nullptr)
{
	sounds.SetLifetimeStrategy(std::make_shared<LcSoundLifetimeStrategy>());

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
	{
		throw std::exception("LcXAudio2System(): Cannot initialize");
	}
}

LcXAudio2System::~LcXAudio2System()
{
	Shutdown();

	CoUninitialize();
}

void LcXAudio2System::Init()
{
	if (FAILED(XAudio2Create(xAudio2.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR)))
	{
		throw std::exception("LcXAudio2System::Init(): Cannot create XAudio2");
	}

	if (FAILED(xAudio2->CreateMasteringVoice(&masteringVoice)))
	{
		throw std::exception("LcXAudio2System::Init(): Cannot create mastering voice");
	}
}

void LcXAudio2System::Shutdown()
{
	LcXAudio2SystemBase::Shutdown();

	if (masteringVoice)
	{
		masteringVoice->DestroyVoice();
		masteringVoice = nullptr;
	}

	xAudio2.Reset();
}

void LcXAudio2System::Update(float deltaSeconds)
{
	LcXAudio2SystemBase::Update(deltaSeconds);
}

ISound* LcXAudio2System::AddSound(const char* filePath)
{
	LcXAudio2Sound* newSound = nullptr;

	LC_TRY

	if (!xAudio2)
	{
		throw std::exception("LcXAudio2System::AddSound(): Invalid audio system");
	}

	newSound = sounds.AddT<LcXAudio2Sound>();
	if (!newSound)
	{
		throw std::exception("LcXAudio2System::AddSound(): Cannot create sound");
	}

	newSound->Load(filePath);

	if (FAILED(xAudio2->CreateSourceVoice(&newSound->voice, newSound->data.getFormat())))
	{
		throw std::exception("LcXAudio2System::AddSound(): Cannot create voice");
	}

	LC_CATCH{ LC_THROW("LcXAudio2System::AddSound()") }

	return newSound;
}

TAudioSystemPtr GetAudioSystem()
{
	return std::make_shared<LcXAudio2System>();
}
