#include <std_include.hpp>

#include <lua.h>

#include <main.h>



#ifndef MENU_TEST

//entrypoint for our Mod
extern "C"
{
	void __declspec(dllexport) init(lua_State* L)
	{
	}
};

#endif




//init strat-tester menu
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		
		DisableThreadLibraryCalls(hModule);
		CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
		
		break;
	case DLL_PROCESS_DETACH:
		kiero::shutdown();
		break;
	}
	return TRUE;
}