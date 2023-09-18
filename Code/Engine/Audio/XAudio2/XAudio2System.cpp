/**
* XAudio2System.cpp
* 18.09.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "Audio/XAudio2/XAudio2System.h"


class LcXAudio2Sound : public ISound
{
public:
	LcXAudio2Sound() : voice(nullptr)
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


public: // ISound interface implementation
	//
	virtual void Update(float deltaSeconds) override {}
	//
	virtual void Play(bool looped) override {}
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


protected:
	IXAudio2SourceVoice* voice;
};


LcXAudio2System::LcXAudio2System() : masteringVoice(nullptr)
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
}

LcXAudio2System::~LcXAudio2System()
{
	Shutdown();
	CoUninitialize();
}

void LcXAudio2System::Init()
{
	if (FAILED(XAudio2Create(xAudio2.GetAddressOf(), 0)))
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
	if (masteringVoice)
	{
		masteringVoice->DestroyVoice();
		masteringVoice = nullptr;
	}

	xAudio2.Reset();
}

void LcXAudio2System::Update(float deltaSeconds)
{
}

ISound* LcXAudio2System::AddSound(const std::string& filePath)
{
	return nullptr;
}

void LcXAudio2System::RemoveSound(ISound* sound)
{
}

TAudioSystemPtr GetAudioSystem()
{
	return std::make_shared<LcXAudio2System>();
}
