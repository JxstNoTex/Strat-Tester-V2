#include "game/game.hpp"
#define WEAK __declspec(selectany)

#define OFFSET(address) (uintptr_t)((address - 0x140000000) + (uintptr_t)GetModuleHandle(NULL))

namespace hks
{
	WEAK game::symbol<void (lua::lua_State* s, const char* libname, const lua::luaL_Reg l[], int nup, int isHksFunc)> hksI_openlib { OFFSET(0x141D49440) }; //updated
	WEAK game::symbol<int (lua::lua_State* s, lua::HksCompilerSettings* options, char const* buff, size_t sz, char const* name)> hksi_hksL_loadbuffer{ OFFSET(0x141D4BD80) }; //updated
	WEAK game::symbol<void (lua::lua_State* s, lua::lua_CFunction fn, int n, const char* functionName, int treatClosureAsFuncForProf)> hks_pushnamedcclosure{ OFFSET(0x141D4BA70) };
	WEAK game::symbol<const char* (lua::lua_State* s, const char* fmt, va_list* argp)> hksi_lua_pushvfstring{ OFFSET(0x141D4E5A0) }; //updated
	WEAK game::symbol<const char* (lua::lua_State* s, lua::HksObject* obj, size_t* len)> hks_obj_tolstring{ OFFSET(0x141D4B6C0 ) }; //updated
	WEAK game::symbol<void (lua::lua_State* s, const char* fmt, ...)> hksi_luaL_error{ OFFSET(0x141D4D050) }; //updated
	WEAK game::symbol<int (lua::lua_State* s, int nargs, int nresults, const lua::hksInstruction* pc)> vm_call_internal{ OFFSET(0x141D70FE0) }; //updated
	WEAK game::symbol<int(lua::lua_State* s, int nargs, int nresults, int errfunc)> hksi_lua_pcall{ OFFSET(0x141D4E390) }; //updated


	WEAK game::symbol<void (const char* error, lua::lua_State* luaVM)> Lua_CoD_LuaStateManager_Error{ OFFSET(0x141F11DA0) }; //updated


	static const char** TypeName = ((const char**)((*(INT64*)(((uintptr_t)GetModuleHandle(NULL) + 0x337B4B8))))); //idk about that one

	int hks_obj_type(const lua::HksObject* obj);
	int hksi_lua_type(lua::lua_State* s, int index);
	int hks_obj_isstring(const lua::HksObject* x);
	int hksi_lua_isstring(lua::lua_State* s, int index);
	int hksi_lua_gettop(lua::lua_State* s);
	int execute_raw_lua(std::string source, const char* chunkName);
}