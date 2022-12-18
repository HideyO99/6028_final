#include "cLuaBrain.h"
#include <iostream>

cLuaBrain::cLuaBrain()
{
	this->m_p_vecGameObj = nullptr;

	this->m_pLuaState = luaL_newstate();

	luaL_openlibs(this->m_pLuaState);

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::Lua_moveObj);
	lua_setglobal(this->m_pLuaState, "setgoto");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::Lua_rotateObj);
	lua_setglobal(this->m_pLuaState, "setrotate");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::Lua_GetObjState);
	lua_setglobal(this->m_pLuaState, "getObjectState");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::Lua_UpdateObjName);
	lua_setglobal(this->m_pLuaState, "updateObjectName");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::Lua_DeathScene);
	lua_setglobal(this->m_pLuaState, "deathscene");

	return;
}

cLuaBrain::~cLuaBrain()
{
    lua_close(this->m_pLuaState);
    return;
}

void cLuaBrain::LoadScript(std::string name, std::string source)
{
	this->m_mapScript[name] = source;
}

void cLuaBrain::DeleteScript(std::string name)
{
	this->m_mapScript.erase(name);
}

void cLuaBrain::SetObjVector(std::vector<cGameObj*>* pvGameObj)
{
	this->m_p_vecGameObj = pvGameObj;
}

void cLuaBrain::Update(float deltaTime)
{
	for (std::map< std::string /*name*/, std::string /*source*/>::iterator itScript =
		this->m_mapScript.begin(); itScript != this->m_mapScript.end(); itScript++)
	{

		// Pass the script into Lua and update
//		int error = luaL_loadbuffer(L, buffer, strlen(buffer), "line");

		std::string curLuaScript = itScript->second;

		int error = luaL_loadstring(this->m_pLuaState,
			curLuaScript.c_str());

		if (error != 0 /*no error*/)
		{
			std::cout << "-------------------------------------------------------" << std::endl;
			std::cout << "Error running Lua script: ";
			std::cout << itScript->first << std::endl;
			std::cout << this->m_decodeLuaErrorToString(error) << std::endl;
			std::cout << "-------------------------------------------------------" << std::endl;
			continue;
		}

		// execute funtion in "protected mode", where problems are 
		//  caught and placed on the stack for investigation
		error = lua_pcall(this->m_pLuaState,	/* lua state */
			0,	/* nargs: number of arguments pushed onto the lua stack */
			0,	/* nresults: number of results that should be on stack at end*/
			0);	/* errfunc: location, in stack, of error function.
					 if 0, results are on top of stack. */
		if (error != 0 /*no error*/)
		{
			std::cout << "Lua: There was an error..." << std::endl;
			std::cout << this->m_decodeLuaErrorToString(error) << std::endl;

			std::string luaError;
			// Get error information from top of stack (-1 is top)
			luaError.append(lua_tostring(this->m_pLuaState, -1));

			// Make error message a little more clear
			std::cout << "-------------------------------------------------------" << std::endl;
			std::cout << "Error running Lua script: ";
			std::cout << itScript->first << std::endl;
			std::cout << luaError << std::endl;
			std::cout << "-------------------------------------------------------" << std::endl;
			// We passed zero (0) as errfunc, so error is on stack)
			lua_pop(this->m_pLuaState, 1);  /* pop error message from the stack */

			continue;
		}

	}
}

void cLuaBrain::RunScriptImmediately(std::string script)
{
	int error = luaL_loadstring(this->m_pLuaState,
		script.c_str());

	if (error != 0 /*no error*/)
	{
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Error running Lua script: ";
		std::cout << this->m_decodeLuaErrorToString(error) << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
		return;
	}

	// execute funtion in "protected mode", where problems are 
	//  caught and placed on the stack for investigation
	error = lua_pcall(this->m_pLuaState,	/* lua state */
		0,	/* nargs: number of arguments pushed onto the lua stack */
		0,	/* nresults: number of results that should be on stack at end*/
		0);	/* errfunc: location, in stack, of error function.
				if 0, results are on top of stack. */
	if (error != 0 /*no error*/)
	{
		std::cout << "Lua: There was an error..." << std::endl;
		std::cout << this->m_decodeLuaErrorToString(error) << std::endl;

		std::string luaError;
		// Get error information from top of stack (-1 is top)
		luaError.append(lua_tostring(this->m_pLuaState, -1));

		// Make error message a little more clear
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Error running Lua script: ";
		std::cout << luaError << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
		// We passed zero (0) as errfunc, so error is on stack)
		lua_pop(this->m_pLuaState, 1);  /* pop error message from the stack */
	}
}
//cmd :  goto(id, x,z)
int cLuaBrain::Lua_moveObj(lua_State* L)
{
	int objectID = (int)lua_tonumber(L, 1);	/* get argument */

	// Exist? 
	cGameObj* pGameObj = cLuaBrain::m_findObjByID(objectID);

	if (pGameObj == nullptr)
	{	// No, it's invalid
		lua_pushboolean(L, false);
		// I pushed 1 thing on stack, so return 1;
		return 1;
	}

	// Object ID is valid

	pGameObj->position.x = (float)lua_tonumber(L, 2);	/* get argument */
	pGameObj->position.z = (float)lua_tonumber(L, 3);
	//pGameObj->position.y = (float)lua_tonumber(L, 3);	/* get argument */
	//pGameObj->position.z = (float)lua_tonumber(L, 4);	/* get argument */
	//pGameObj->velocity.x = (float)lua_tonumber(L, 5);	/* get argument */
	//pGameObj->velocity.y = (float)lua_tonumber(L, 6);	/* get argument */
	//pGameObj->velocity.z = (float)lua_tonumber(L, 7);	/* get argument */
	pGameObj->direction.x = pGameObj->position.x - pGameObj->pMeshObj->position.x;
	pGameObj->direction.z = pGameObj->position.z - pGameObj->pMeshObj->position.z;
	lua_pushboolean(L, true);	// index is OK

	return 1;		// There were 7 things on the stack
}

int cLuaBrain::Lua_rotateObj(lua_State* L)
{
	int objectID = (int)lua_tonumber(L, 1);	/* get argument */

	// Exist? 
	cGameObj* pGameObj = cLuaBrain::m_findObjByID(objectID);

	if (pGameObj == nullptr)
	{	// No, it's invalid
		lua_pushboolean(L, false);
		// I pushed 1 thing on stack, so return 1;
		return 1;
	}

	// Object ID is valid

	pGameObj->rotation.y = (float)lua_tonumber(L, 2);	/* get argument */

	lua_pushboolean(L, true);	// index is OK

	return 1;		
}

int cLuaBrain::Lua_GetObjState(lua_State* L)
{
	int objectID = (int)lua_tonumber(L, 1);	/* get argument */

	// Exist? 
	cGameObj* pGameObj = cLuaBrain::m_findObjByID(objectID);

	if (pGameObj == nullptr)
	{	// No, it's invalid
		lua_pushboolean(L, false);
		//		lua_pushstring(L, "Didn't work. So sorry");
				// I pushed 1 thing on stack, so return 1;
		return 1;
	}

	// Object ID is valid

	lua_pushboolean(L, true);	// index is OK		// 1st item on stack
	lua_pushnumber(L, pGameObj->position.x);			// 2nd item on stack
	lua_pushnumber(L, pGameObj->position.y);			// 3rd item
	lua_pushnumber(L, pGameObj->position.z);			// and so on
	lua_pushnumber(L, pGameObj->velocity.x);
	lua_pushnumber(L, pGameObj->velocity.y);
	lua_pushnumber(L, pGameObj->velocity.z);

	return 7;		// There were 7 things on the stack
}

std::vector< cGameObj* >* cLuaBrain::m_p_vecGameObj;

int cLuaBrain::Lua_UpdateObjName(lua_State* L)
{
	std::cout << "UpdateObjectName() was called from calling updateObjectName() in Lua" << std::endl;

	int objectID = (int)lua_tonumber(L, 1);	


	cGameObj* pGameObj = cLuaBrain::m_findObjByID(objectID);

	if (pGameObj == nullptr)
	{	
		lua_pushboolean(L, false);
		return 1;
	}

	char* newName = (char*)lua_tostring(L, 2);

	std::string strNewName;
	strNewName.append(newName);	

	pGameObj->name = strNewName;

	return 0;
}

int cLuaBrain::Lua_DeathScene(lua_State* L)
{
	int objectID = (int)lua_tonumber(L, 1);	/* get argument */

	// Exist? 
	cGameObj* pGameObj = cLuaBrain::m_findObjByID(objectID);

	if (pGameObj == nullptr)
	{	// No, it's invalid
		lua_pushboolean(L, false);
		//		lua_pushstring(L, "Didn't work. So sorry");
				// I pushed 1 thing on stack, so return 1;
		return 1;
	}
	pGameObj->rotation.y += 1.57;
	pGameObj->scale *= 0.9;
	return 0;
}


cGameObj* cLuaBrain::m_findObjByID(int ID)
{
	for (std::vector<cGameObj*>::iterator itGO = cLuaBrain::m_p_vecGameObj->begin();
		itGO != cLuaBrain::m_p_vecGameObj->end(); itGO++)
	{
		if ((*itGO)->getID() == ID)
		{	// Found it!
			return (*itGO);
		}
	}
	return nullptr;
}

std::string cLuaBrain::m_decodeLuaErrorToString(int error)
{
	switch (error)
	{
	case 0:
		return "Lua: no error";
		break;
	case LUA_ERRSYNTAX:
		return "Lua: syntax error";
		break;
	case LUA_ERRMEM:
		return "Lua: memory allocation error";
		break;
	case LUA_ERRRUN:
		return "Lua: Runtime error";
		break;
	case LUA_ERRERR:
		return "Lua: Error while running the error handler function";
		break;
	}


	return "Lua: UNKNOWN error";
}
