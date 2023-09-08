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

void LcBox2DWorld::AddStaticBox(LcVector2 pos, LcSizef size)
{
    b2BodyDef bodyDef;
    bodyDef.position.Set(pos.x, pos.y);
    b2Body* body = box2DWorld->CreateBody(&bodyDef);

    b2PolygonShape box;
    box.SetAsBox(size.x / 2.0f, size.y / 2.0f);
    body->CreateFixture(&box, 0.0f);
}

b2Body* LcBox2DWorld::AddDynamicBox(LcVector2 pos, LcSizef size, bool fixedRotation)
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(pos.x, pos.y);
    bodyDef.fixedRotation = fixedRotation;
    b2Body* body = box2DWorld->CreateBody(&bodyDef);

    b2PolygonShape box;
    box.SetAsBox(32.0f, 32.0f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &box;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    body->CreateFixture(&fixtureDef);

    return body;
}
