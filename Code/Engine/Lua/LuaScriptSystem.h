/**
* LuaScriptSystem.h
* 24.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/InputSystem.h"
#include "Core/ScriptSystem.h"
#include "World/WorldInterface.h"
#include "World/SpriteInterface.h"
#include "Application/ApplicationInterface.h"

#pragma warning(disable : 4275)

#define lua_tofloat(L,i) (float)lua_tonumberx(L,(i),NULL)
#define lua_toint(L,i)   (int)lua_tointegerx(L,(i),NULL)

// throws exception if can't get
LCLUA_API IScriptSystem* GetScript(struct lua_State* luaState);

// throws exception if can't get
LCLUA_API IApplication* GetApp(struct lua_State* luaState);

// throws exception if can't get
LCLUA_API IWorld* GetWorld(struct lua_State* luaState);

// throws exception if can't get
LCLUA_API IInputSystem* GetInput(struct lua_State* luaState);

// throws exception if can't get
LCLUA_API LcColor4 GetColor(struct lua_State* luaState, int table);

// throws exception if can't get
LCLUA_API LcVector2 GetVector2(struct lua_State* luaState, int table);

// throws exception if can't get
LCLUA_API LcVector3 GetVector(struct lua_State* luaState, int table);


/**
* Lua script system */
class LCLUA_API LcLuaScriptSystem : public IScriptSystem
{
public:
	//
	LcLuaScriptSystem(bool inOpenBaseDefaultLibs = true, bool inOpenAllDefaultLibs = false)
		: openBaseDefaultLibs(inOpenBaseDefaultLibs)
		, openAllDefaultLibs(inOpenAllDefaultLibs)
		, luaState(nullptr)
	{}
	//
	struct lua_State* GetState() { return luaState; }


public: // IScriptSystem interface implementation
	//
	~LcLuaScriptSystem();
	//
	virtual void Init(const LcAppContext& context) override;
	//
	virtual void RunScript(const std::string& script) override;
	//
	virtual void RunScriptFile(const char* filePath) override;
	//
	virtual LcAny RunScriptEx(const std::string& script) override;
	//
	virtual LcAny RunScriptFileEx(const char* filePath) override;
	//
	virtual void RunUpdateHandler(float deltaSeconds) override;
	//
	virtual void RunActionsHandler(const std::string& action) override;
	//
	virtual void RunKeysHandler(int key, LcKeyState keyEvent) override;
	//
	virtual void RunAxisHandler(int axis, float x, float y) override;
	//
	virtual void SetHandlerName(LcScriptHandler type, const char* name) override;
	//
	virtual const char* GetHandlerName(LcScriptHandler type) const override;


public:
	LcLuaScriptSystem(const LcLuaScriptSystem&) = delete;
	//
	LcLuaScriptSystem& operator=(const LcLuaScriptSystem&) = delete;


protected:
	struct lua_State* luaState;
	//
	bool openBaseDefaultLibs;
	//
	bool openAllDefaultLibs;
	//
	std::string updateHandlerName;
	//
	std::string actionsHandlerName;
	//
	std::string keysHandlerName;
	//
	std::string axisHandlerName;

};
