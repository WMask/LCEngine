/**
* Audio.h
* 18.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Core/LCCreator.h"
#include "Core/LCTypes.h"

#include <functional>
#include <string>
#include <memory>
#include <deque>


/** Playback end handler */
typedef std::function<void(class ISound&)> LcPlaybackEndHandler;

/**
* Playable sound interface */
class ISound : public IObjectBase
{
public:
	/**
	* Virtual destructor */
	virtual ~ISound() {}
	/**
	* Set streamed sound end handler */
	virtual void SetStreamEndHandler(LcPlaybackEndHandler handler) = 0;
	/**
	* Update sound */
	virtual void Update(float deltaSeconds, const LcAppContext& context) = 0;
	/**
	* Play sound */
	virtual void Play() = 0;
	/**
	* Stop playing */
	virtual void Stop() = 0;
	/**
	* Pause playing */
	virtual void Pause() = 0;
	/**
	* Set volume. Default: 1.0 */
	virtual void SetVolume(float volume) = 0;
	/**
	* Get sound playing state */
	virtual bool IsPlaying() const = 0;
	/**
	* Get sound paused state */
	virtual bool IsPaused() const = 0;
	/**
	* Get sound streamed state. Wav - not streamed, Ogg - streamed. */
	virtual bool IsStreamed() const = 0;


protected:
	/**
	* Load sound */
	virtual void Load(const char* filePath, class IAudioSystem* audio) = 0;

};


/**
* Audio system interface */
class IAudioSystem
{
public:
	typedef std::shared_ptr<ISound> TSoundPtr;
	//
	typedef std::deque<TSoundPtr> TSoundsList;


public:
	/**
	* Virtual destructor */
	virtual ~IAudioSystem() {}
	/**
	* Initialize system */
	virtual void Init(const LcAppContext& context) = 0;
	/**
	* Shutdown system */
	virtual void Shutdown() = 0;
	/**
	* Remove all sounds */
	virtual void Clear(bool removeRooted = false) = 0;
	/**
	* Update system */
	virtual void Update(float deltaSeconds, const LcAppContext& context) = 0;
	/**
	* Add sound */
	virtual ISound* AddSound(const char* filePath) = 0;
	/**
	* Remove sound */
	virtual void RemoveSound(ISound* sound) = 0;
	/**
	* Get sound */
	virtual ISound* GetSoundByTag(ObjectTag tag) const = 0;
	/**
	* Get sounds list */
	virtual const TSoundsList& GetSounds() const = 0;

};


/**
* Audio system base class */
template<class T>
class LcAudioSystemBase : public IAudioSystem
{
public:// IAudioSystem interface implementation
	//
	virtual ~LcAudioSystemBase() override
	{
		Shutdown();
	}
	//
	virtual void Shutdown() override
	{
		sounds.Clear();
	}
	//
	virtual void Update(float deltaSeconds, const LcAppContext& context) override
	{
		for (auto& sound : sounds.GetItems()) sound->Update(deltaSeconds, context);
	}
	//
	virtual void RemoveSound(ISound* sound) override
	{
		sounds.Remove(sound);
	}
	//
	virtual const TSoundsList& GetSounds() const override { return sounds.GetItems(); }


protected:
	LcCreator<ISound> sounds;

};
