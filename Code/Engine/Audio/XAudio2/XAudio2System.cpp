/**
* XAudio2System.cpp
* 18.09.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "Audio/XAudio2/XAudio2System.h"
#include "Audio/XAudio2/XAudio2Sound.h"
#include "Audio/RiffFile.h"
#include "Core/LCException.h"


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

void LcXAudio2System::Init(const LcAppContext& context)
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

void LcXAudio2System::Update(float deltaSeconds, const LcAppContext& context)
{
	LcXAudio2SystemBase::Update(deltaSeconds, context);
}

ISound* LcXAudio2System::AddSound(const char* filePath)
{
	LcXAudio2Sound* newSound = nullptr;

	LC_TRY

	if (!xAudio2)
	{
		throw std::exception("LcXAudio2System::AddSound(): Invalid audio system");
	}

	newSound = sounds.Add<LcXAudio2Sound>();
	if (!newSound)
	{
		throw std::exception("LcXAudio2System::AddSound(): Cannot create sound");
	}

	newSound->Load(filePath, xAudio2.Get());

	LC_CATCH{ LC_THROW("LcXAudio2System::AddSound()") }

	return newSound;
}

TAudioSystemPtr GetAudioSystem()
{
	return std::make_shared<LcXAudio2System>();
}
