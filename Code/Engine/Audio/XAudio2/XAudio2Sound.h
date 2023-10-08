/**
* XAudio2Sound.h
* 19.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/Audio.h"
#include "Audio/RiffFile.h"
#include "Audio/OggFile.h"

#include <wrl.h>
#include <xaudio2.h>

using namespace Microsoft::WRL;

#pragma warning(disable : 4251)
#pragma warning(disable : 4275)


namespace LcCreatables { constexpr int XAudio2Sound = 0; }


/**
* XAudio2 sound */
class LcXAudio2Sound : public ISound
{
public:
	LcXAudio2Sound();
	//
	virtual ~LcXAudio2Sound() override;
	//
	static int GetStaticId() { return LcCreatables::XAudio2Sound; }


public: // ISound interface implementation
	//
	virtual void SetStreamEndHandler(LcPlaybackEndHandler handler) override { streamedEndHandler = handler; }
	//
	virtual void Load(const char* filePath, class IAudioSystem* audio) override;
	//
	virtual void Update(float deltaSeconds, const LcAppContext& context) override;
	//
	virtual void Play() override;
	//
	virtual void Stop() override;
	//
	virtual void Pause() override;
	//
	virtual void SetVolume(float volume) override;
	//
	virtual bool IsPlaying() const override { return playing; }
	//
	virtual bool IsPaused() const override { return paused; }
	//
	virtual bool IsStreamed() const override { return streamed; }


protected:
	//
	void InitStreamedBuffers();


protected:
	//
	LcPlaybackEndHandler streamedEndHandler;
	//
	IXAudio2SourceVoice* voice;
	//
	XAUDIO2_BUFFER xBuffer;
	//
	LcRiffFile riffBuffer;
	//
	LcOggFile oggBuffer;
	//
	bool streamed;
	//
	bool playing;
	//
	bool paused;

};
