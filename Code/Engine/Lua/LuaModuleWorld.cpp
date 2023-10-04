/**
* LuaModuleWorld.cpp
* 30.09.2023
* (c) Denis Romakhov
*/

#include "Lua/LuaScriptSystem.h"
#include "World/SpriteInterface.h"
#include "GUI/WidgetInterface.h"
#include "Core/LCUtils.h"
#include "Core/Visual.h"

#include "src/lua.hpp"


static int AddSprite(lua_State* luaState)
{
	bool visible = true;
	float x, y, z = 0.0f, width, height, rotation = 0.0f;
	int top = lua_gettop(luaState);

	if (lua_istable(luaState, top - 4))
	{
		x = lua_tofloat(luaState, top - 6);
		y = lua_tofloat(luaState, top - 5);

		lua_getfield(luaState, top - 4, "z");
		if (!lua_isnumber(luaState, -1)) throw std::exception("AddSprite(): Invalid table");
		z = lua_tofloat(luaState, -1);
		lua_pop(luaState, 1);

		width = lua_tofloat(luaState, top - 3);
		height = lua_tofloat(luaState, top - 2);
		rotation = lua_tofloat(luaState, top - 1);
		visible = (lua_toboolean(luaState, top - 0) != 0);
	}
	else
	{
		x = lua_tofloat(luaState, top - 5);
		y = lua_tofloat(luaState, top - 4);
		width = lua_tofloat(luaState, top - 3);
		height = lua_tofloat(luaState, top - 2);
		rotation = lua_tofloat(luaState, top - 1);
		visible = (lua_toboolean(luaState, top - 0) != 0);
	}

	auto world = GetWorld(luaState);
	if (world)
	{
		auto sprite = world->AddSprite(x, y, LcLayersRange(z), width, height, rotation, visible);
		lua_pushlightuserdata(luaState, sprite);
	}
	else throw std::exception("AddSprite(): Invalid World");

	return 1;
}

static int AddWidget(lua_State* luaState)
{
	bool visible = true;
	float x, y, z = 0.0f, width, height;
	int top = lua_gettop(luaState);

	if (lua_istable(luaState, top - 3))
	{
		x = lua_tofloat(luaState, top - 5);
		y = lua_tofloat(luaState, top - 4);

		lua_getfield(luaState, top - 3, "z");
		if (!lua_isnumber(luaState, -1)) throw std::exception("AddWidget(): Invalid table");
		z = lua_tofloat(luaState, -1);
		lua_pop(luaState, 1);

		width = lua_tofloat(luaState, top - 2);
		height = lua_tofloat(luaState, top - 1);
		visible = (lua_toboolean(luaState, top - 0) != 0);
	}
	else
	{
		x = lua_tofloat(luaState, top - 4);
		y = lua_tofloat(luaState, top - 3);
		width = lua_tofloat(luaState, top - 2);
		height = lua_tofloat(luaState, top - 1);
		visible = (lua_toboolean(luaState, top - 0) != 0);
	}

	auto world = GetWorld(luaState);
	if (world)
	{
		auto widget = world->AddWidget(x, y, LcLayersRange(z), width, height, visible);
		lua_pushlightuserdata(luaState, widget);
	}
	else throw std::exception("AddWidget(): Invalid World");

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

LcBasicParticleSettings GetParticleSettings(struct lua_State* luaState, int table);

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

LcTextBlockSettings GetTextBlockSettings(struct lua_State* luaState, int table);

static int AddTextComponent(lua_State* luaState)
{
	IWidget* widget = nullptr;
	LcTextBlockSettings settings;
	std::string textUtf8;
	int top = lua_gettop(luaState);

	if (lua_isuserdata(luaState, top - 2))
	{
		widget = static_cast<IWidget*>(lua_touserdata(luaState, top - 2));
		textUtf8 = lua_tolstring(luaState, top - 1, 0);
		settings = GetTextBlockSettings(luaState, top - 0);
	}
	else
	{
		textUtf8 = lua_tolstring(luaState, top - 1, 0);
		settings = GetTextBlockSettings(luaState, top - 0);
	}

	if (widget)
	{
		auto app = GetApp(luaState);
		widget->AddTextComponent(app->GetContext(), FromUtf8(textUtf8), settings);
	}
	else
	{
		auto world = GetWorld(luaState);
		world->GetWidgetHelper().AddTextComponent(FromUtf8(textUtf8), settings);
	}

	return 0;
}

static int AddButtonComponent(lua_State* luaState)
{
	IWidget* widget = nullptr;
	std::string texture;
	LcVector2 idlePos, overPos, presPos;
	int top = lua_gettop(luaState);

	if (lua_isuserdata(luaState, top - 4))
	{
		widget = static_cast<IWidget*>(lua_touserdata(luaState, top - 4));
		texture = lua_tolstring(luaState, top - 3, 0);
		idlePos = GetVector2(luaState, top - 2);
		overPos = GetVector2(luaState, top - 1);
		presPos = GetVector2(luaState, top - 0);
	}
	else
	{
		texture = lua_tolstring(luaState, top - 3, 0);
		idlePos = GetVector2(luaState, top - 2);
		overPos = GetVector2(luaState, top - 1);
		presPos = GetVector2(luaState, top - 0);
	}

	if (widget)
	{
		auto app = GetApp(luaState);
		widget->AddButtonComponent(app->GetContext(), texture, idlePos, overPos, presPos);
	}
	else
	{
		auto world = GetWorld(luaState);
		world->GetWidgetHelper().AddButtonComponent(texture, idlePos, overPos, presPos);
	}

	return 0;
}

static int AddCheckboxComponent(lua_State* luaState)
{
	IWidget* widget = nullptr;
	std::string texture;
	LcVector2 uncheckedPos, uncheckedHoveredPos, checkedPos, checkedHoveredPos;
	int top = lua_gettop(luaState);

	if (lua_isuserdata(luaState, top - 5))
	{
		widget = static_cast<IWidget*>(lua_touserdata(luaState, top - 5));
		texture = lua_tolstring(luaState, top - 4, 0);
		uncheckedPos = GetVector2(luaState, top - 3);
		uncheckedHoveredPos = GetVector2(luaState, top - 2);
		checkedPos = GetVector2(luaState, top - 1);
		checkedHoveredPos = GetVector2(luaState, top - 0);
	}
	else
	{
		texture = lua_tolstring(luaState, top - 4, 0);
		uncheckedPos = GetVector2(luaState, top - 3);
		uncheckedHoveredPos = GetVector2(luaState, top - 2);
		checkedPos = GetVector2(luaState, top - 1);
		checkedHoveredPos = GetVector2(luaState, top - 0);
	}

	if (widget)
	{
		auto app = GetApp(luaState);
		widget->AddCheckboxComponent(app->GetContext(), texture, uncheckedPos, uncheckedHoveredPos, checkedPos, checkedHoveredPos);
	}
	else
	{
		auto world = GetWorld(luaState);
		world->GetWidgetHelper().AddCheckboxComponent(texture, uncheckedPos, uncheckedHoveredPos, checkedPos, checkedHoveredPos);
	}

	return 0;
}

static int AddClickHandlerComponent(lua_State* luaState)
{
	IWidget* widget = nullptr;
	std::string handlerName;
	int top = lua_gettop(luaState);

	if (lua_isuserdata(luaState, top - 1))
	{
		widget = static_cast<IWidget*>(lua_touserdata(luaState, top - 1));
		handlerName = lua_tolstring(luaState, top - 0, 0);
	}
	else
	{
		handlerName = lua_tolstring(luaState, top - 0, 0);
	}

	if (handlerName.empty()) throw std::exception("AddClickHandlerComponent(): Invalid handler name");

	auto handler = [luaState, handlerName]() {
		lua_getglobal(luaState, handlerName.c_str());
		lua_call(luaState, 0, 0);
	};

	if (widget)
	{
		auto app = GetApp(luaState);
		widget->AddClickHandlerComponent(app->GetContext(), handler);
	}
	else
	{
		auto world = GetWorld(luaState);
		world->GetWidgetHelper().AddClickHandlerComponent(handler);
	}

	return 0;
}

static int AddCheckHandlerComponent(lua_State* luaState)
{
	IWidget* widget = nullptr;
	std::string handlerName;
	int top = lua_gettop(luaState);

	if (lua_isuserdata(luaState, top - 1))
	{
		widget = static_cast<IWidget*>(lua_touserdata(luaState, top - 1));
		handlerName = lua_tolstring(luaState, top - 0, 0);
	}
	else
	{
		handlerName = lua_tolstring(luaState, top - 0, 0);
	}

	if (handlerName.empty()) throw std::exception("AddCheckHandlerComponent(): Invalid handler name");

	auto handler = [luaState, handlerName](bool checked) {
		lua_getglobal(luaState, handlerName.c_str());
		lua_pushboolean(luaState, (checked ? 1 : 0));
		lua_call(luaState, 1, 0);
	};

	if (widget)
	{
		auto app = GetApp(luaState);
		widget->AddCheckHandlerComponent(app->GetContext(), handler);
	}
	else
	{
		auto world = GetWorld(luaState);
		world->GetWidgetHelper().AddCheckHandlerComponent(handler);
	}

	return 0;
}


void AddLuaModuleWorld(const LcAppContext& context, IScriptSystem* scriptSystem)
{
	auto luaSystem = static_cast<LcLuaScriptSystem*>(context.scripts);
	auto luaSystemCustom = static_cast<LcLuaScriptSystem*>(scriptSystem);
	auto luaState = luaSystem ? (luaSystemCustom ? luaSystemCustom->GetState() : luaSystem->GetState()) : nullptr;
	if (!luaState) throw std::exception("AddLuaModuleWorld(): Invalid Lua state");

	for (int i = 1; i <= 9; i++)
	{
		std::string layerName = "Z" + ToString(i);
		float value = static_cast<float>(i) / -10.0f;
		lua_pushnumber(luaState, value);
		lua_setglobal(luaState, layerName.c_str());
	}

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

	lua_pushcfunction(luaState, AddWidget);
	lua_setglobal(luaState, "AddWidget");

	lua_pushcfunction(luaState, AddTextComponent);
	lua_setglobal(luaState, "AddTextComponent");

	lua_pushcfunction(luaState, AddButtonComponent);
	lua_setglobal(luaState, "AddButtonComponent");

	lua_pushcfunction(luaState, AddCheckboxComponent);
	lua_setglobal(luaState, "AddCheckboxComponent");

	lua_pushcfunction(luaState, AddClickHandlerComponent);
	lua_setglobal(luaState, "AddClickHandlerComponent");

	lua_pushcfunction(luaState, AddCheckHandlerComponent);
	lua_setglobal(luaState, "AddCheckHandlerComponent");
}


LcBasicParticleSettings GetParticleSettings(struct lua_State* luaState, int table)
{
	if (!lua_istable(luaState, table)) throw std::exception("AddParticlesComponent(): Invalid table");

	LcBasicParticleSettings settings;

	lua_getfield(luaState, table, "frameSize");
	if (!lua_istable(luaState, -1)) throw std::exception("AddParticlesComponent(): Invalid table");

	{
		lua_getfield(luaState, -1, "x");
		if (!lua_isnumber(luaState, -1)) throw std::exception("AddParticlesComponent(): Invalid table");
		settings.frameSize.x = lua_tofloat(luaState, -1);
		lua_pop(luaState, 1);

		lua_getfield(luaState, -1, "y");
		if (!lua_isnumber(luaState, -1)) throw std::exception("AddParticlesComponent(): Invalid table");
		settings.frameSize.y = lua_tofloat(luaState, -1);
		lua_pop(luaState, 1);
	}

	lua_pop(luaState, 1);

	lua_getfield(luaState, table, "numFrames");
	if (!lua_isnumber(luaState, -1)) throw std::exception("AddParticlesComponent(): Invalid table");
	settings.numFrames = lua_toint(luaState, -1);
	lua_pop(luaState, 1);

	lua_getfield(luaState, table, "movementRadius");
	if (!lua_isnumber(luaState, -1)) throw std::exception("AddParticlesComponent(): Invalid table");
	settings.movementRadius = lua_tofloat(luaState, -1);
	lua_pop(luaState, 1);

	lua_getfield(luaState, table, "lifetime");
	settings.lifetime = lua_isnumber(luaState, -1) ? lua_tofloat(luaState, -1) : -1.0f;
	lua_pop(luaState, 1);

	lua_getfield(luaState, table, "fadeInRate");
	settings.fadeInRate = lua_isnumber(luaState, -1) ? lua_tofloat(luaState, -1) : -1.0f;
	lua_pop(luaState, 1);

	lua_getfield(luaState, table, "fadeOutRate");
	settings.fadeOutRate = lua_isnumber(luaState, -1) ? lua_tofloat(luaState, -1) : -1.0f;
	lua_pop(luaState, 1);

	lua_getfield(luaState, table, "speed");
	settings.speed = lua_isnumber(luaState, -1) ? lua_tofloat(luaState, -1) : 1.0f;
	lua_pop(luaState, 1);

	return settings;
}

LcTextBlockSettings GetTextBlockSettings(struct lua_State* luaState, int table)
{
	if (!lua_istable(luaState, table)) throw std::exception("GetTextBlockSettings(): Invalid table");

	LcTextBlockSettings settings;

	lua_getfield(luaState, table, "textColor");
	if (!lua_istable(luaState, -1)) throw std::exception("AddParticlesComponent(): Invalid table");
	settings.textColor = GetColor(luaState, -1);
	lua_pop(luaState, 1);

	lua_getfield(luaState, table, "textAlign");
	if (!lua_isstring(luaState, -1)) throw std::exception("AddParticlesComponent(): Invalid table");
	settings.textAlign = ToAlignment(lua_tolstring(luaState, -1, 0));
	lua_pop(luaState, 1);

	lua_getfield(luaState, table, "fontName");
	if (!lua_isstring(luaState, -1)) throw std::exception("AddParticlesComponent(): Invalid table");
	settings.fontName = FromUtf8(lua_tolstring(luaState, -1, 0));
	lua_pop(luaState, 1);

	lua_getfield(luaState, table, "fontWeight");
	if (!lua_isstring(luaState, -1)) throw std::exception("AddParticlesComponent(): Invalid table");
	settings.fontWeight = ToWeight(lua_tolstring(luaState, -1, 0));
	lua_pop(luaState, 1);

	lua_getfield(luaState, table, "fontSize");
	if (!lua_isinteger(luaState, -1)) throw std::exception("AddParticlesComponent(): Invalid table");
	settings.fontSize = lua_toint(luaState, -1);
	lua_pop(luaState, 1);

	return settings;
}
