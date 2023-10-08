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

#include <algorithm>
#include <iterator>


class LcSoundLifetimeStrategy : public LcLifetimeStrategy<ISound, IAudioSystem::TSoundsList>
{
public:
	LcSoundLifetimeStrategy() {}
	//
	virtual ~LcSoundLifetimeStrategy() {}
	//
	virtual std::shared_ptr<ISound> Create(const void* userData) override { return std::make_shared<LcXAudio2Sound>(); }
	//
	virtual void Destroy(ISound& item, IAudioSystem::TSoundsList& items) override {}
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

void LcXAudio2System::Clear(bool removeRooted)
{
	if (removeRooted)
	{
		sounds.Clear();
	}
	else
	{
		IAudioSystem::TSoundsList removedSounds;
		auto& soundsList = sounds.GetItems();

		std::copy_if(soundsList.begin(), soundsList.end(), std::inserter(removedSounds, removedSounds.begin()), [](auto& sound) {
			return !sound->IsRooted();
		});

		sounds.Clear(removedSounds.begin(), removedSounds.end());
	}
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

	newSound->Load(filePath, this);

	LC_CATCH{ LC_THROW("LcXAudio2System::AddSound()") }

	return newSound;
}

ISound* LcXAudio2System::GetSoundByTag(ObjectTag tag) const
{
	auto it = std::find_if(sounds.GetItems().begin(), sounds.GetItems().end(), [tag](auto& sound) {
		return sound->GetTag() == tag;
	});
	return (it != sounds.GetItems().end()) ? it->get() : nullptr;
}


TAudioSystemPtr GetAudioSystem()
{
	return std::make_shared<LcXAudio2System>();
}
