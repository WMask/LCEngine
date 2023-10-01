/**
* LuaModuleWorld.cpp
* 30.09.2023
* (c) Denis Romakhov
*/

#include "Lua/LuaScriptSystem.h"
#include "World/SpriteInterface.h"
#include "GUI/WidgetInterface.h"
#include "Core/Visual.h"

#include "src/lua.hpp"


static int AddSprite(lua_State* luaState)
{
	float x, y, z = 0.0f, width, height;
	int top = lua_gettop(luaState);

	if (lua_istable(luaState, top - 2))
	{
		x = lua_tofloat(luaState, top - 4);
		y = lua_tofloat(luaState, top - 3);

		lua_getfield(luaState, top - 2, "z");
		if (!lua_isnumber(luaState, -1)) throw std::exception("AddSprite(): Invalid table");
		z = lua_tofloat(luaState, -1);
		lua_pop(luaState, 1);

		width = lua_tofloat(luaState, top - 1);
		height = lua_tofloat(luaState, top - 0);
	}
	else
	{
		x = lua_tofloat(luaState, top - 3);
		y = lua_tofloat(luaState, top - 2);
		width = lua_tofloat(luaState, top - 1);
		height = lua_tofloat(luaState, top - 0);
	}

	auto world = GetWorld(luaState);
	if (world)
	{
		auto sprite = world->AddSprite(x, y, LcLayersRange(z), width, height);
		lua_pushlightuserdata(luaState, sprite);
	}
	else throw std::exception("AddSprite(): Invalid World");

	return 1;
}

static int AddTintComponent(lua_State* luaState)
{
	IVisual* visual = nullptr;
	LcColor4 tint{};
	int top = lua_gettop(luaState);

	if (lua_isuserdata(luaState, top))
	{
		visual = static_cast<IVisual*>(lua_touserdata(luaState, top));
		tint = GetColor(luaState, top - 1);
	}
	else
	{
		tint = GetColor(luaState, top);
	}

	if (visual)
	{
		auto app = GetApp(luaState);
		visual->AddTintComponent(app->GetContext(), tint);
	}
	else
	{
		auto world = GetWorld(luaState);
		world->GetVisualHelper().AddTintComponent(tint);
	}

	return 0;
}

static int AddColorsComponent(lua_State* luaState)
{
	IVisual* visual = nullptr;
	LcColor4 leftTop, rightTop, rightBottom, leftBottom;
	int top = lua_gettop(luaState);

	if (lua_isuserdata(luaState, top - 4))
	{
		visual = static_cast<IVisual*>(lua_touserdata(luaState, top - 4));
		leftTop = GetColor(luaState, top - 3);
		rightTop = GetColor(luaState, top - 2);
		rightBottom = GetColor(luaState, top - 1);
		leftBottom = GetColor(luaState, top - 0);
	}
	else
	{
		leftTop = GetColor(luaState, top - 3);
		rightTop = GetColor(luaState, top - 2);
		rightBottom = GetColor(luaState, top - 1);
		leftBottom = GetColor(luaState, top - 0);
	}

	if (visual)
	{
		auto app = GetApp(luaState);
		visual->AddColorsComponent(app->GetContext(), leftTop, rightTop, rightBottom, leftBottom);
	}
	else
	{
		auto world = GetWorld(luaState);
		world->GetVisualHelper().AddColorsComponent(leftTop, rightTop, rightBottom, leftBottom);
	}

	return 0;
}

static int AddTextureComponent(lua_State* luaState)
{
	IVisual* visual = nullptr;
	const char* texture = nullptr;
	int top = lua_gettop(luaState);

	if (lua_isuserdata(luaState, top))
	{
		visual = static_cast<IVisual*>(lua_touserdata(luaState, top));
		texture = lua_tolstring(luaState, top - 1, 0);
	}
	else
	{
		texture = lua_tolstring(luaState, top - 0, 0);
	}

	if (visual)
	{
		auto app = GetApp(luaState);
		visual->AddTextureComponent(app->GetContext(), texture);
	}
	else
	{
		auto world = GetWorld(luaState);
		world->GetVisualHelper().AddTextureComponent(texture);
	}

	return 0;
}

static int AddCustomUVComponent(lua_State* luaState)
{
	ISprite* sprite = nullptr;
	LcVector2 leftTop, rightTop, rightBottom, leftBottom;
	int top = lua_gettop(luaState);

	if (lua_isuserdata(luaState, top - 4))
	{
		sprite = static_cast<ISprite*>(lua_touserdata(luaState, top - 4));
		leftTop = GetVector2(luaState, top - 3);
		rightTop = GetVector2(luaState, top - 2);
		rightBottom = GetVector2(luaState, top - 1);
		leftBottom = GetVector2(luaState, top - 0);
	}
	else
	{
		leftTop = GetVector2(luaState, top - 3);
		rightTop = GetVector2(luaState, top - 2);
		rightBottom = GetVector2(luaState, top - 1);
		leftBottom = GetVector2(luaState, top - 0);
	}

	if (sprite)
	{
		auto app = GetApp(luaState);
		sprite->AddCustomUVComponent(app->GetContext(), leftTop, rightTop, rightBottom, leftBottom);
	}
	else
	{
		auto world = GetWorld(luaState);
		world->GetSpriteHelper().AddCustomUVComponent(leftTop, rightTop, rightBottom, leftBottom);
	}

	return 0;
}

static int AddAnimationComponent(lua_State* luaState)
{
	ISprite* sprite = nullptr;
	LcVector2 frameSize{};
	float framesPerSecond;
	int numFrames;
	int top = lua_gettop(luaState);

	if (lua_isuserdata(luaState, top - 3))
	{
		sprite = static_cast<ISprite*>(lua_touserdata(luaState, top - 3));
		frameSize = GetVector2(luaState, top - 2);
		numFrames = lua_toint(luaState, top - 1);
		framesPerSecond = lua_tofloat(luaState, top - 0);
	}
	else
	{
		frameSize = GetVector2(luaState, top - 2);
		numFrames = lua_toint(luaState, top - 1);
		framesPerSecond = lua_tofloat(luaState, top - 0);
	}

	if (sprite)
	{
		auto app = GetApp(luaState);
		sprite->AddAnimationComponent(app->GetContext(), frameSize, numFrames, framesPerSecond);
	}
	else
	{
		auto world = GetWorld(luaState);
		world->GetSpriteHelper().AddAnimationComponent(frameSize, numFrames, framesPerSecond);
	}

	return 0;
}

static int AddTiledComponent(lua_State* luaState)
{
	ISprite* sprite = nullptr;
	const char* tilesPath = nullptr;
	int top = lua_gettop(luaState);

	if (lua_isuserdata(luaState, top))
	{
		sprite = static_cast<ISprite*>(lua_touserdata(luaState, top));
		tilesPath = lua_tolstring(luaState, top - 1, 0);
	}
	else
	{
		tilesPath = lua_tolstring(luaState, top - 0, 0);
	}

	if (sprite)
	{
		auto app = GetApp(luaState);
		sprite->AddTiledComponent(app->GetContext(), tilesPath);
	}
	else
	{
		auto world = GetWorld(luaState);
		world->GetSpriteHelper().AddTiledComponent(tilesPath);
	}

	return 0;
}

static int AddParticlesComponent(lua_State* luaState)
{
	ISprite* sprite = nullptr;
	LcBasicParticleSettings settings;
	int numSprites;
	int top = lua_gettop(luaState);

	if (lua_isuserdata(luaState, top - 2))
	{
		sprite = static_cast<ISprite*>(lua_touserdata(luaState, top - 2));
		numSprites = lua_toint(luaState, top - 1);
		settings = GetParticleSettings(luaState, top - 0);
	}
	else
	{
		numSprites = lua_toint(luaState, top - 1);
		settings = GetParticleSettings(luaState, top - 0);
	}

	if (sprite)
	{
		auto app = GetApp(luaState);
		sprite->AddParticlesComponent(app->GetContext(), numSprites, settings);
	}
	else
	{
		auto world = GetWorld(luaState);
		world->GetSpriteHelper().AddParticlesComponent(numSprites, settings);
	}

	return 0;
}

void AddLuaModuleWorld(const LcAppContext& context, IScriptSystem* scriptSystem)
{
	auto luaSystem = static_cast<LcLuaScriptSystem*>(context.scripts);
	auto luaSystemCustom = static_cast<LcLuaScriptSystem*>(scriptSystem);
	auto luaState = luaSystem ? (luaSystemCustom ? luaSystemCustom->GetState() : luaSystem->GetState()) : nullptr;
	if (!luaState) throw std::exception("AddLuaModuleWorld(): Invalid Lua state");

	lua_pushlightuserdata(luaState, context.world);
	lua_setglobal(luaState, LuaWorldGlobalName);

	lua_pushcfunction(luaState, AddSprite);
	lua_setglobal(luaState, "AddSprite");

	lua_pushcfunction(luaState, AddTintComponent);
	lua_setglobal(luaState, "AddTintComponent");

	lua_pushcfunction(luaState, AddColorsComponent);
	lua_setglobal(luaState, "AddColorsComponent");

	lua_pushcfunction(luaState, AddTextureComponent);
	lua_setglobal(luaState, "AddTextureComponent");

	lua_pushcfunction(luaState, AddCustomUVComponent);
	lua_setglobal(luaState, "AddCustomUVComponent");

	lua_pushcfunction(luaState, AddAnimationComponent);
	lua_setglobal(luaState, "AddAnimationComponent");

	lua_pushcfunction(luaState, AddTiledComponent);
	lua_setglobal(luaState, "AddTiledComponent");

	lua_pushcfunction(luaState, AddParticlesComponent);
	lua_setglobal(luaState, "AddParticlesComponent");
}
