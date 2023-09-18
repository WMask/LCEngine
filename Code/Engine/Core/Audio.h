/**
* Audio.h
* 18.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Core/LCTypesEx.h"

#include <string>
#include <memory>
#include <deque>


/**
* Playable sound interface */
class ISound
{
public:
	/**
	* Virtual destructor */
	virtual ~ISound() {}
	/**
	* Update sound */
	virtual void Update(float deltaSeconds) = 0;
	/**
	* Play sound from start */
	virtual void Play(bool looped = false) = 0;
	/**
	* Stop playing */
	virtual void Stop() = 0;
	/**
	* Pause playing */
	virtual void Pause() = 0;
	/**
	* Resume playing */
	virtual void Resume() = 0;
	/**
	* Get sound playing state */
	virtual bool IsPlaying() = 0;
	/**
	* Get sound paused state */
	virtual bool IsPaused() = 0;
	/**
	* Get sound streamed state. Wav - not streamed, Ogg - streamed. */
	virtual bool IsStreamed() = 0;

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
	virtual void Init() = 0;
	/**
	* Shutdown system */
	virtual void Shutdown() = 0;
	/**
	* Update system */
	virtual void Update(float deltaSeconds) = 0;
	/**
	* Add sound */
	virtual ISound* AddSound(const char* filePath) = 0;
	/**
	* Remove sound */
	virtual void RemoveSound(ISound* sound) = 0;
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
		sounds.GetList().clear();
	}
	//
	virtual void Update(float deltaSeconds) override
	{
		for (auto& sound : sounds.GetList()) sound->Update(deltaSeconds);
	}
	//
	virtual void RemoveSound(ISound* sound) override
	{
		return sounds.Remove(sound);
	}
	//
	virtual const TSoundsList& GetSounds() const override { return sounds.GetList(); }


protected:
	LcCreator<ISound, T> sounds;

};

typedef std::shared_ptr<IAudioSystem> TAudioSystemPtr;
