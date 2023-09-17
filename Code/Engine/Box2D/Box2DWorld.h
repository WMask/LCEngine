/**
* Box2DWorld.h
* 08.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/Physics.h"

#include <memory>

#pragma warning(disable : 4251)
#pragma warning(disable : 4275)


/** Box2D config */
struct LcBox2DConfig
{
	LcBox2DConfig(LcVector2 inGravity, int inVelocityIterations = 8, int inPositionIterations = 3)
		: gravity(inGravity), velocityIterations(inVelocityIterations), positionIterations(inPositionIterations)
	{
	}
	LcBox2DConfig(float gravity)
		: gravity(LcVector2(0.0f, gravity)), velocityIterations(8), positionIterations(3)
	{
	}
	LcVector2 gravity;
	//
	int velocityIterations;
	//
	int positionIterations;
};


/**
* Box2D game world */
class BOX2D_API LcBox2DWorld : public IPhysicsWorld
{
public:
	LcBox2DWorld(const LcBox2DConfig& config);
	//
	LcBox2DWorld(float gravity);
	//
	~LcBox2DWorld();


public:// IPhysicsWorld interface implementation
	//
	virtual void Clear() override;
	//
	virtual void Update(float deltaSeconds) override;
	//
	virtual void AddStaticBox(LcVector2 pos, LcSizef size) override;
	//
	virtual IPhysicsBody* AddDynamic(LcVector2 pos, float radius, float density, bool fixedRotation = true) override;
	//
	virtual IPhysicsBody* AddDynamicBox(LcVector2 pos, LcSizef size, float density, bool fixedRotation = true) override;
	//
	virtual const TBodiesList& GetDynamicBodies() const override { return dynamicBodies.GetList(); }
	//
	virtual TBodiesList& GetDynamicBodies() override { return dynamicBodies.GetList(); }


protected:
	std::unique_ptr<class b2World> box2DWorld;
	//
	LcCreator<IPhysicsBody, class LcBox2DBody> dynamicBodies;
	//
	LcBox2DConfig config;

};
