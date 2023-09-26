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

/** Key state: Up, Down */
enum class LcKeyState
{
	Up,
	Down
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


constexpr int LcKeysCount = 150;

/** Keys struct */
struct CORE_API KEYS
{
	KEYS();
	//
	unsigned char* Get() { return keys; }
	//
	const unsigned char* Get() const { return keys; }
	//
	unsigned char& operator[](int index);
	//
	unsigned char keys[LcKeysCount];
};

/** Any value container */
struct LcAny
{
	enum class LcAnyType { None, StringAny, FloatAny, IntAny, BoolAny };
	LcAny() : type(LcAnyType::None), fValue(0.0f), iValue(0), bValue(false) {}
	LcAny(const std::string& value) : type(LcAnyType::StringAny), sValue(value), fValue(0.0f), iValue(0), bValue(false) {}
	LcAny(float value) : type(LcAnyType::FloatAny), fValue(value), iValue(0), bValue(false) {}
	LcAny(int value) : type(LcAnyType::IntAny), fValue(0.0f), iValue(value), bValue(false) {}
	LcAny(bool value) : type(LcAnyType::BoolAny), fValue(0.0f), iValue(value), bValue(value) {}
	//
	std::string	sValue;
	float		fValue;
	bool		bValue;
	int			iValue;
	LcAnyType   type;
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
	virtual void RemoveAll() = 0;
	//
	virtual const TItemsList& GetList() const = 0;
	//
	virtual TItemsList& GetList() = 0;

};

/** Application context */
struct LcAppContext
{
	LcAppContext() : app(nullptr), world(nullptr),
		render(nullptr), audio(nullptr), scripts(nullptr),
		input(nullptr), gui(nullptr), physics(nullptr),
		windowHandle(nullptr) {}
	//
	class IApplication* app;
	//
	class IWorld* world;
	//
	class IRenderSystem* render;
	//
	class IScriptSystem* scripts;
	//
	class IAudioSystem* audio;
	//
	class IInputSystem* input;
	//
	class IGuiManager* gui;
	//
	class IPhysicsWorld* physics;
	//
	void* windowHandle;
};
