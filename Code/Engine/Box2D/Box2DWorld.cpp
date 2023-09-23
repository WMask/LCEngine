/**
* Box2DWorld.cpp
* 08.09.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "Box2D/Box2DWorld.h"
#include "World/WorldInterface.h"
#include "Core/LCException.h"
#include "Core/LCUtils.h"

// put Box2D library into Code/Engine/Box2D folder
#include "box2d/box2d.h"

static const float BOX2D_SCALE = 100.0f;


inline LcVector2 ToLC(const b2Vec2& v, bool scale = true)
{
    return LcVector2(v.x * (scale ? BOX2D_SCALE : 1.0f), v.y * (scale ? BOX2D_SCALE : 1.0f));
}
inline b2Vec2 FromLC(const LcVector2& v, bool scale = true)
{
    return b2Vec2(v.x / (scale ? BOX2D_SCALE : 1.0f), v.y / (scale ? BOX2D_SCALE : 1.0f));
}

struct LcBodyQueryHandler : public b2QueryCallback
{
    LcBodyQueryHandler(bool inCheckStaticOnly) : found(false), checkStaticOnly(inCheckStaticOnly) {}
    //
    virtual ~LcBodyQueryHandler() {}
    //
    virtual bool ReportFixture(b2Fixture* fixture) override
    {
        if (checkStaticOnly)
        {
            bool dynamic = (fixture && fixture->GetBody() && fixture->GetBody()->GetType() == b2BodyType::b2_dynamicBody);
            auto pos = fixture->GetBody()->GetPosition();
            if (dynamic) return true;
        }

        found = true;
        return false;
    }
    //
    bool found = false;
    //
    bool checkStaticOnly = false;
};

class LcBox2DBody : public IPhysicsBody
{
public:
    LcBox2DBody() : world(nullptr), body(nullptr), size(LcDefaults::ZeroSize) {}
	//
    ~LcBox2DBody() {}
    //
    void Init(b2World* inWorld, b2Body* inBody, b2Fixture* inFixture, LcSizef inSize)
    {
        world = inWorld;
        body = inBody;
        fixture = inFixture;
        size.x = inSize.x / BOX2D_SCALE;
        size.y = inSize.y / BOX2D_SCALE;
    }
    //
    b2World* world;
    //
    b2Fixture* fixture;
    //
    b2Body* body;
    //
    LcSizef size;


public:// IPhysicsBody interface implementation
	//
	virtual void ApplyForce(LcVector2 force) override { body->ApplyForceToCenter(FromLC(force, false), true); }
    //
    virtual void ApplyImpulse(LcVector2 impulse) override { body->ApplyLinearImpulseToCenter(FromLC(impulse, false), true); }
	//
	virtual void SetVelocity(LcVector2 velocity) override { body->SetLinearVelocity(FromLC(velocity, false)); }
    //
    virtual LcVector2 GetVelocity() const override { return ToLC(body->GetLinearVelocity(), false); }
	//
	virtual LcVector2 GetPos() const override { return ToLC(body->GetPosition()); }
	//
	virtual float GetRotation() const override { return body->GetAngle(); }
    //
    virtual void SetUserData(void* data) override { body->GetUserData().pointer = reinterpret_cast<uintptr_t>(data); }
    //
    virtual void* GetUserData() const override { return reinterpret_cast<void*>(body->GetUserData().pointer); }
    //
    virtual bool IsFalling(float depth, bool checkStaticOnly) const override
    {
        auto pos = body->GetPosition();
        b2AABB area{
            {pos.x - size.x / 2.0f, pos.y + size.y / 2.0f + 0.01f},
            {pos.x + size.x / 2.0f, pos.y + size.y / 2.0f + 0.01f + depth / BOX2D_SCALE}
        };

        LcBodyQueryHandler handler(checkStaticOnly);
        world->QueryAABB(&handler, area);

        return !handler.found;
    }
};

class LcBodyLifetimeStrategy : public LcLifetimeStrategy<IPhysicsBody>
{
public:
    LcBodyLifetimeStrategy() {}
    //
    virtual ~LcBodyLifetimeStrategy() {}
    //
    virtual std::shared_ptr<IPhysicsBody> Create() override { return std::make_shared<LcBox2DBody>(); }
    //
    virtual void Destroy(IPhysicsBody& item) override
    {
        LcBox2DBody& body = static_cast<LcBox2DBody&>(item);
        body.body->DestroyFixture(body.fixture);
        body.world->DestroyBody(body.body);
    }
};


LcBox2DWorld::LcBox2DWorld(const LcBox2DConfig& inConfig) : config(inConfig)
{
    box2DWorld = std::make_unique<b2World>(FromLC(config.gravity, false));
    dynamicBodies.SetLifetimeStrategy(std::make_shared<LcBodyLifetimeStrategy>());
}

LcBox2DWorld::LcBox2DWorld(float gravity) : LcBox2DWorld(LcBox2DConfig(gravity))
{
}

LcBox2DWorld::~LcBox2DWorld()
{
}

void LcBox2DWorld::Clear()
{
    dynamicBodies.GetList().clear();
    box2DWorld = std::make_unique<b2World>(FromLC(config.gravity, false));
}

void LcBox2DWorld::Update(float deltaSeconds)
{
	if (box2DWorld) box2DWorld->Step(deltaSeconds, config.velocityIterations, config.positionIterations);
}

void LcBox2DWorld::AddStaticBox(LcVector2 pos, LcSizef size)
{
    if (!box2DWorld) throw std::exception("LcBox2DWorld::AddStaticBox(): Invalid world");

    b2BodyDef bodyDef;
    bodyDef.position.Set(pos.x / BOX2D_SCALE, pos.y / BOX2D_SCALE);
    b2Body* body = box2DWorld->CreateBody(&bodyDef);
    if (!body) throw std::exception("LcBox2DWorld::AddStaticBox(): Cannot create body");

    b2PolygonShape box;
    box.SetAsBox(size.x / BOX2D_SCALE / 2.0f, size.y / BOX2D_SCALE / 2.0f);
    body->CreateFixture(&box, 0.0f);
}

IPhysicsBody* LcBox2DWorld::AddDynamicBox(LcVector2 pos, LcSizef size, float density, bool fixedRotation)
{
    if (!box2DWorld) throw std::exception("LcBox2DWorld::AddDynamicBox(): Invalid world");

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(pos.x / BOX2D_SCALE, pos.y / BOX2D_SCALE);
    bodyDef.fixedRotation = fixedRotation;
    b2Body* body = box2DWorld->CreateBody(&bodyDef);
    if (!body) throw std::exception("LcBox2DWorld::AddDynamicBox(): Cannot create body");

    b2PolygonShape shape;
    shape.SetAsBox(size.x / BOX2D_SCALE / 2.0f, size.y / BOX2D_SCALE / 2.0f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = density;
    fixtureDef.friction = 0.3f;
    b2Fixture* fixture = body->CreateFixture(&fixtureDef);
    if (!fixture) throw std::exception("LcBox2DWorld::AddDynamicBox(): Cannot create fixture");

    auto newBody = dynamicBodies.AddT<LcBox2DBody>();
    if (!newBody) throw std::exception("LcBox2DWorld::AddDynamicBox(): Cannot create dynamic body");

    newBody->Init(box2DWorld.get(), body, fixture, size);

    return newBody;
}

IPhysicsBody* LcBox2DWorld::AddDynamic(LcVector2 pos, float radius, float density, bool fixedRotation)
{
    if (!box2DWorld) throw std::exception("LcBox2DWorld::AddDynamic(): Invalid world");

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(pos.x / BOX2D_SCALE, pos.y / BOX2D_SCALE);
    bodyDef.fixedRotation = fixedRotation;
    b2Body* body = box2DWorld->CreateBody(&bodyDef);
    if (!body) throw std::exception("LcBox2DWorld::AddDynamic(): Cannot create body");

    b2CircleShape shape;
    shape.m_radius = radius / BOX2D_SCALE;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = density;
    fixtureDef.friction = 0.3f;
    b2Fixture* fixture = body->CreateFixture(&fixtureDef);
    if (!fixture) throw std::exception("LcBox2DWorld::AddDynamic(): Cannot create fixture");

    LcSizef size(radius * 2.0f, radius * 2.0f);
    auto newBody = dynamicBodies.AddT<LcBox2DBody>();
    if (!newBody) throw std::exception("LcBox2DWorld::AddDynamic(): Cannot create dynamic body");

    newBody->Init(box2DWorld.get(), body, fixture, size);

    return newBody;
}

TPhysicsWorldPtr GetPhysicsWorld()
{
    return std::make_shared<LcBox2DWorld>(10.0f);
}
