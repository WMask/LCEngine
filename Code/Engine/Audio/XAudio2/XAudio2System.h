/**
* XAudio2System.h
* 18.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/Audio.h"

#include <wrl.h>
#include <xaudio2.h>

using namespace Microsoft::WRL;

#pragma warning(disable : 4251)
#pragma warning(disable : 4275)


class LcXAudio2Sound;
using LcXAudio2SystemBase = LcAudioSystemBase<LcXAudio2Sound>;

/**
* XAudio2 audio system */
class LcXAudio2System : public LcXAudio2SystemBase
{
public:
	LcXAudio2System();
	//
	virtual ~LcXAudio2System() override;


public:// IAudioSystem interface implementation
	//
	virtual void Init(const LcAppContext& context) override;
	//
	virtual void Shutdown() override;
	//
	virtual void Update(float deltaSeconds, const LcAppContext& context) override;
	//
	virtual ISound* AddSound(const char* filePath) override;
	//
	virtual void RemoveSounds() override { sounds.Clear(); }


protected:
	ComPtr<IXAudio2> xAudio2;
	//
	IXAudio2MasteringVoice* masteringVoice;

};
