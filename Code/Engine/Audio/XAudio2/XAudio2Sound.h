/**
* XAudio2Sound.h
* 19.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/Audio.h"
#include "Audio/RiffFile.h"

#include <wrl.h>
#include <xaudio2.h>

using namespace Microsoft::WRL;

#pragma warning(disable : 4251)
#pragma warning(disable : 4275)


/**
* XAudio2 sound */
class LcXAudio2Sound : public ISound
{
public:
	LcXAudio2Sound();
	//
	virtual ~LcXAudio2Sound() override;
	//
	void Load(const char* filePath, IXAudio2* audio);


public: // ISound interface implementation
	//
	virtual void Update(float deltaSeconds, const LcAppContext& context) override;
	//
	virtual void Play(bool looped) override;
	//
	virtual void Stop() override;
	//
	virtual void Pause() override;
	//
	virtual void Resume() override;
	//
	virtual bool IsPlaying() override;
	//
	virtual bool IsPaused() override;
	//
	virtual bool IsStreamed() override { return streamed; }


protected:
	//
	IXAudio2SourceVoice* voice;
	//
	XAUDIO2_BUFFER xBuffers[2];
	//
	LcRiffFile dataBuffers[2];
	//
	int currentBuffer;
	//
	bool streamed;

};
