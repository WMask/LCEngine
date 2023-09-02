/**
* Camera.h
* 28.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Core/LcTypesEx.h"


/**
* Game camera */
class LcCamera
{
public:
	/**
	* Constructor */
	LcCamera() : position(LcDefaults::ZeroVec3), target(LcDefaults::ZeroVec3) {}
	/**
	* Set camera */
	inline void Set(LcVector3 newPos, LcVector3 newTarget)
	{
		position = newPos;
		target = newTarget;
	}
	/**
	* Move camera */
	inline void Move(LcVector3 newPosOffset, LcVector3 newTargetOffset)
	{
		position = position + newPosOffset;
		target = target + newTargetOffset;
	}
	/**
	* Get position */
	inline LcVector3 GetPosition() const { return position; }
	/**
	* Get target */
	inline LcVector3 GetTarget() const { return target; }


protected:
	LcVector3 position;
	//
	LcVector3 target;

};
