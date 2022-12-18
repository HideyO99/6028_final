#pragma once

extern "C" {
#include <Lua5.4.4/lua.h>
#include <Lua5.4.4/lualib.h>
#include <Lua5.4.4/lauxlib.h>
}

#include <string>
#include <vector>
#include <map>
#include "..\GameObj\cGameObj.h"

class cLuaBrain
{
public:
	cLuaBrain();
	~cLuaBrain();

	void LoadScript(std::string name, std::string source);
	void DeleteScript(std::string name);
	// Passes a pointer to the game object vector
	void SetObjVector(std::vector<cGameObj*>* pvGameObj);
	// Call all the active scripts that are loaded
	void Update(float deltaTime);

	void RunScriptImmediately(std::string script);
	// called "setObjectState" in lua
	// Something like this: "setObjectState( 24, 0, 0, 0, 0, 0, 0 )"
	static int Lua_moveObj( lua_State* L);

	static int Lua_rotateObj(lua_State* L);
	// called "getObjectState" in lua
	static int Lua_GetObjState(lua_State* L);
	// Lua script name is "updateObjectName"
// Example: updateObjectName( ID, newName )
	static int Lua_UpdateObjName(lua_State* L);
	static int Lua_DeathScene(lua_State* L);

private:
	std::map<std::string, std::string> m_mapScript; //<name,source>
	static std::vector<cGameObj*>* m_p_vecGameObj;
	static cGameObj* m_findObjByID(int ID);
	lua_State* m_pLuaState;
	std::string m_decodeLuaErrorToString(int error);
};

