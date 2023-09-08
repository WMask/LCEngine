/**
* Box2DWorld.h
* 08.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/LCTypesEx.h"

#include <memory>

#pragma warning(disable : 4251)


/** Box2D config */
struct LcBox2DConfig
{
	LcBox2DConfig(LcVector2 inGravity, int inVelocityIterations = 8, int inPositionIterations = 3)
		: gravity(inGravity), velocityIterations(inVelocityIterations), positionIterations(inPositionIterations)
	{
	}
	LcBox2DConfig(float gravity)
		: gravity(LcVector2(0.0f, -gravity)), velocityIterations(8), positionIterations(3)
	{
	}
	LcVector2 gravity;
	//
	int velocityIterations;
	//
	int positionIterations;
};


/**
* Box2D game world
*/
class BOX2D_API LcBox2DWorld
{
public:
	LcBox2DWorld(const LcBox2DConfig& config);
	//
	LcBox2DWorld(float gravity);
	//
	~LcBox2DWorld();
	//
	void Update(float deltaSeconds);
	//
	void AddStaticBox(LcVector2 pos, LcSizef size);
	//
	class b2Body* AddDynamicBox(LcVector2 pos, LcSizef size, bool fixedRotation = true);


protected:
	std::unique_ptr<class b2World> box2DWorld;
	//
	LcBox2DConfig config;

};