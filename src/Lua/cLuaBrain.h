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
	void SetObjVector(std::vector<cGameObj*>* pvGameObj);
	void Update(float deltaTime);

	void RunScriptImmediately(std::string script);
	static int Lua_UpdateObj( lua_State* L);
	static int Lua_GetObjState(lua_State* L);
	static int Lua_UpdateObjName(lua_State* L);


private:
	std::map<std::string, std::string> m_mapScript; //<name,source>
	static std::vector<cGameObj*>* m_p_vecGameObj;
	static cGameObj* m_findObjByID(int ID);
	lua_State* m_pLuaState;
	std::string m_decodeLuaErrorToString(int error);
};

