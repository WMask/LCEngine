/**
* Physics.h
* 09.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/LCTypesEx.h"
#include "Core/LCCreator.h"

#include <memory>
#include <deque>


/**
* Physics body */
class IPhysicsBody
{
public:
	/**
	* Destructor */
	virtual ~IPhysicsBody() {}
	/**
	* Apply force */
	virtual void ApplyForce(LcVector2 force) = 0;
	/**
	* Apply force */
	virtual void ApplyImpulse(LcVector2 impulse) = 0;
	/**
	* Set velocity */
	virtual void SetVelocity(LcVector2 velocity) = 0;
	/**
	* Get velocity */
	virtual LcVector2 GetVelocity() const = 0;
	/**
	* Get position */
	virtual LcVector2 GetPos() const = 0;
	/**
	* Get rotation in radians */
	virtual float GetRotation() const = 0;
	/**
	* Set user data */
	virtual void SetUserData(void* data) = 0;
	/**
	* Get user data */
	virtual void* GetUserData() const = 0;
	/**
	* Get user object */
	template<class T> T* GetUserObject() const { return static_cast<T*>(GetUserData()); }
	/**
	* Check area below the body. Depth - bottom offset in pixels */
	virtual bool IsFalling(float depth = 2.0f, bool checkStaticOnly = true) const = 0;

};


/**
* Physics world */
class IPhysicsWorld
{
public:
	typedef std::shared_ptr<IPhysicsBody> TBodyPtr;
	//
	typedef std::deque<TBodyPtr> TBodiesList;


public:
	/**
	* Destructor */
	virtual ~IPhysicsWorld() {}
	/**
	* Remove all physics objects */
	virtual void Clear() = 0;
	/**
	* Update world */
	virtual void Update(float deltaSeconds) = 0;
	/**
	* Add static box */
	virtual void AddStaticBox(LcVector2 pos, LcSizef size) = 0;
	/**
	* Add dynamic sphere body */
	virtual IPhysicsBody* AddDynamic(LcVector2 pos, float radius, float density = 1.0f, bool fixedRotation = true) = 0;
	/**
	* Add dynamic box body */
	virtual IPhysicsBody* AddDynamicBox(LcVector2 pos, LcSizef size, float density = 1.0f, bool fixedRotation = true) = 0;
	/**
	* Remove all dynamic bodies */
	virtual void RemoveAllBodies() = 0;
	/**
	* Get dynamic body list */
	virtual const TBodiesList& GetDynamicBodies() const = 0;
	/**
	* Get dynamic body list */
	virtual TBodiesList& GetDynamicBodies() = 0;

};
