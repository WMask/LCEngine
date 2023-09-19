/**
* LCTypes.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"

#include <vector>
#include <string>
#include <memory>
#include <deque>


/** vector of unsigned chars */
typedef std::vector<unsigned char> LcBytes;


/** Mouse buttons */
enum class LcMouseBtn
{
	Left,
	Right,
	Middle
};

/** Key state */
enum class LcKeyState
{
	Down,
	Up
};

/** Render system type */
enum class LcRenderSystemType
{
	Null,
	DX7,
	DX9,
	DX10
};

/** Window mode */
enum class LcWinMode : int { Windowed, Fullscreen };


/** Keys struct */
struct CORE_API KEYS
{
	KEYS();
	//
	unsigned char& operator[](int index);
	//
	const static int numKeys = 128;
	//
	unsigned char keys[numKeys];
};

/** Any value container */
struct LcAny
{
	LcAny() : fValue(0.0f), iValue(0), bValue(false) {}
	//
	std::string	sValue;
	float		fValue;
	bool		bValue;
	int			iValue;
};

CORE_API float LcClamp(float value, float minValue, float maxValue);
CORE_API int LcClamp(int value, int minValue, int maxValue);

/** Range struct */
template<typename T, const T& minValue, const T& maxValue>
struct LcRange
{
	LcRange() : value(static_cast<T>(0)) {}
	//
	LcRange(const LcRange& ref) : value(ref.value) {}
	//
	LcRange(T inValue) : value(LcClamp(inValue, minValue, maxValue)) {}
	//
	inline LcRange& operator = (const LcRange& ref) { value = ref.value; return *this; }
	//
	inline operator T () const { return value; }
	//
	T value;
};

/** Object creator interface */
template<class T>
class ICreator
{
public:
	typedef std::deque<std::shared_ptr<T>> TItemsList;


public:
	virtual T* Add() = 0;
	//
	virtual void Remove(T* item) = 0;
	//
	virtual const TItemsList& GetList() const = 0;
	//
	virtual TItemsList& GetList() = 0;

};

/** Application context */
struct LcAppContext
{
	LcAppContext(class IWorld& inWorld) : world(inWorld), render(nullptr), scripts(nullptr), physics(nullptr) {}
	//
	class IWorld* GetWorldPtr() const { return &world; }
	//
	class IWorld& world;
	//
	class IRenderSystem* render;
	//
	class IScriptSystem* scripts;
	//
	class IAudioSystem* audio;
	//
	class IPhysicsWorld* physics;
};
