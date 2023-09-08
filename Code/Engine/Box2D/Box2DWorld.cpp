/**
* Box2DWorld.cpp
* 08.09.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "Box2D/Box2DWorld.h"
// put Box2D library into Code/Engine/Box2D folder
#include "box2d/box2d.h"


LcBox2DWorld::LcBox2DWorld(const LcBox2DConfig& inConfig) : config(inConfig)
{
	box2DWorld = std::make_unique<b2World>(b2Vec2(config.gravity.x, config.gravity.y));
}

LcBox2DWorld::LcBox2DWorld(float gravity) : LcBox2DWorld(LcBox2DConfig(gravity))
{
}

LcBox2DWorld::~LcBox2DWorld()
{
}

void LcBox2DWorld::Update(float deltaSeconds)
{
	box2DWorld->Step(deltaSeconds, config.velocityIterations, config.positionIterations);
}
