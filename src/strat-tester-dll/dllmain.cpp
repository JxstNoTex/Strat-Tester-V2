#include <std_include.hpp>


#include "builtins.h"
#include "Opcodes.h"
#include "detours.h"
#include "inject.h"
#include <main.h>
#include "game/game.hpp"
#include "havok/hks_api.hpp"
#include "loader/component_loader.hpp"

injector inject;

int init()
{
	//create object reference for injector

	inject.injectT7();


	return 1;
}

void unload()
{
	//destroy console
	component_loader::pre_destroy();


	//unload resource and gsc

	inject.FreeT7();

	ScriptDetours::ResetDetours();
	BOOL gsiResult = UnlockResource(inject.Hres_GSI);
	gsiResult = FreeResource(inject.Hres_GSI);

	BOOL gccResult1 = UnlockResource(inject.Hres_GSCC);
	gccResult1 = FreeResource(inject.Hres_GSCC);
}

#ifndef MENU_TEST

//entrypoint for our Mod
extern "C"
{
	int __declspec(dllexport) init(lua::lua_State* L)
	{
		game::minlog.WriteLine("T7Overchared initiating");

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
		GSCBuiltins::Init();
		ScriptDetours::InstallHooks();
		Opcodes::Init();
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