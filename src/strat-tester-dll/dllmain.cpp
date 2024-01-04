#include <std_include.hpp>



#include <main.h>
#include "game/game.hpp"
#include "havok/hks_api.hpp"
#include "loader/component_loader.hpp"
#include "components/arxan.hpp"


int init()
{
	return 1;
}

void unload()
{
	//destroy console
	component_loader::pre_destroy();
}

#ifndef MENU_TEST

//entrypoint for our Mod
extern "C"
{
	int __declspec(dllexport) init(lua::lua_State* L)
	{
		game::minlog.WriteLine("T7Overchared initiating");
		arxan::InstallExceptionDispatcher();
		arxan::search_and_patch_integrity_checks();
		const lua::luaL_Reg T7OverchargedLibrary[] =
		{
			{nullptr, nullptr},
		};
		hks::hksI_openlib(L, "T7Overcharged", T7OverchargedLibrary, 0, 1);

		if (!component_loader::post_start())
		{
			game::Com_Error_("", 0, 0x200u, "Error while loading T7Overcharged components");
			game::minlog.WriteLine("Error while loading T7Overcharged components");
			return 0;
		}

		game::minlog.WriteLine("T7Overchared initiated");

		game::LoadDvarHashMap();
		return 1;
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

		init();
		__security_init_cookie();
		DisableThreadLibraryCalls(hModule);
		atexit(unload);
		CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
		
		
		break;
	case DLL_PROCESS_DETACH:
		kiero::shutdown();
		break;
	}
	return TRUE;
}