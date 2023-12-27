#pragma once

#define WEAK __declspec(selectany)

namespace game
{
#define REBASE(address) (uintptr_t)((address - 0x140000000) + game::base)
#define OFFSET(address) (uintptr_t)((address - 0x140000000) + (uintptr_t)GetModuleHandle(NULL))

	/* BG */
	//WEAK symbol<int(void* ps, void* weapon/*const playerState_t* ps, const Weapon weapon*/)> BG_GetAmmoInClip{ OFFSET(0x1426E7310) };
	//WEAK symbol<int(void* ps, void* weapon/*const playerState_t* ps, const Weapon weapon*/)> BG_GetTotalAmmoReserve{ OFFSET(0x1426E7AB0) };
	//WEAK symbol<int(void* weapon/*const Weapon weapon*/)> BG_GetClipSize{ OFFSET(0x1426E75D0) };
	//WEAK symbol<bool(void* weapon/*const Weapon weapon*/)> BG_IsLeftHandWeapon{ OFFSET(0x1426F4A90) };
	//WEAK symbol<bool(void* weapon/*const Weapon weapon*/)> BG_IsDualWield{ OFFSET(0x1426F4820) };
	//WEAK symbol<void* (void* weapon/*const Weapon weapon*/)> BG_GetDualWieldWeapon{ OFFSET(0x1426EFFD0) };

	/* BUILT-IN */
	WEAK symbol<const char* (const char* s0, const char* substr)> I_stristr{ OFFSET(0x1422E96F0) }; //updated
	WEAK symbol<__int64(int* a1, __int64 a2, __int64 a3)> PLmemset{ OFFSET(0x142C3DE60) }; //updated
	WEAK symbol<int(char* string, unsigned int count, const char* format, char* ap)> vsnprintf{ OFFSET(0x142C3CF70) }; //updated

	/* CG */
	WEAK symbol<void(void* obj, const game::XAnimNotifyInfo* notifyInfo, void* info, const unsigned int notifyFilter, bool shutdown, bool skipNonImportantNotifies)> CG_ProcessClientNote{ OFFSET(0x140255C70) }; //updated

	/* CMD */
	// Variables
	WEAK symbol<game::cmd_function_t*> cmd_functions{ OFFSET(0x15689DF58) }; //updated

	/* COM */
	WEAK symbol<char(const char* file, int line, lua::ErrorCode code, const char* fmt, ...)> Com_Error_{ OFFSET(0x1420F8170) }; //updated
	WEAK symbol<const char* (const char* name)> Com_GameInfo_GetGameTypeRefCaps{ OFFSET(0x1420F3630) }; //updated
	WEAK symbol<const char* (const char* name)> Com_GameInfo_GetGameTypeRef{ OFFSET(0x1420F35B0) }; //updated
	WEAK symbol<const char* (const char* name)> Com_GameInfo_GetMapRef{ OFFSET(0x1420F3820) }; //updated
	WEAK symbol<bool()> Com_IsRunningUILevel{ OFFSET(0x142148350) }; //updated
	WEAK symbol<bool(consoleChannel_e channel, consoleLabel_e label, const char* fmt, ...)> Com_Printf{ OFFSET(0x142148F60) }; //updated
	WEAK symbol<bool(consoleChannel_e channel, consoleLabel_e label, const char* msg, int error)> Com_PrintMessage{ OFFSET(0x142148C00) }; //updated
	WEAK symbol<bool(eGameModes mode)> Com_SessionMode_IsGameMode{ OFFSET(0x1420F7330) }; //updated
	WEAK symbol<bool(eModes mode)> Com_SessionMode_IsMode{ OFFSET(0x1420F7370) }; //updated
	WEAK symbol<int(char* dest, size_t size, const char* fmt, ...)> Com_sprintf{ OFFSET(0x142C3CA60) }; //updated

	/* CSC/GSC */
	WEAK symbol<ScrString_t(const char* s)> GScr_AllocString{ OFFSET(0x141A83520) }; //updated
	WEAK symbol<int(scriptInstance_t inst, unsigned int index)> Scr_GetInt{ OFFSET(0x1412EB7F0) }; //updated
	WEAK symbol<const char* (scriptInstance_t inst, unsigned int index)> Scr_GetString{ OFFSET(0x1412EBAA0) }; //updated
	WEAK symbol<int(int scriptInst, int var)> ScrVm_AddInt{ OFFSET(0x1412E9870) }; //7FF6439E0000 / 1404478E0
	WEAK symbol<void(int scriptInst,  char* str)> ScrVm_AddString{ OFFSET(0x1412E9A30) };
	WEAK symbol<INT64(game::XAssetType type, char* name, bool errorifmissing, int waitTime)>DB_FindXAssetHeader{ 0x141420ED0_g };
	WEAK symbol<INT64(INT32 canonID, INT32* type, INT32* min_args, INT32* max_args)>Scr_GetFunction{ 0x141AF7820_g };
	WEAK symbol<INT64(INT32 canonID, INT32* type, INT32* min_args, INT32* max_args)>Scr_GetMethod{ 0x141AF79B0_g };
	//WEAK symbol<float(int scriptInst, float var)> ScrVm_AddFloat{ 0x1412E9760_g };


	// Variables
	WEAK symbol<BuiltinFunctionDef> isProfileBuildFunctionDef{ OFFSET(0x1432D7D70) }; //updated

	/* DB */
	WEAK symbol<size_t(XAssetType type)> DB_GetXAssetTypeSize{ OFFSET(0x1413E9DE0) }; //updated
	// Variables
	static XAssetPool* DB_XAssetPool = reinterpret_cast<XAssetPool*>(OFFSET(0x1494073F0)); //updated
	static bool s_luaInFrontend = ((bool)OFFSET(0x143413BD8)); //updated
	static lua::lua_State* UI_luaVM = ((lua::lua_State*)((*(INT64*)OFFSET(0x159C76D88)))); //updated

	/* DObj */
	WEAK symbol<int(void*/*const DObj*/ obj, ScrString_t name, BoneIndex* index, int modelNum)> DObjGetBoneIndex{ OFFSET(0x14233D3B0) }; //updated
	WEAK symbol<LocalClientNum_t(void*/*const DObj*/ obj)> DObjGetLocalClientIndex{ OFFSET(0x142336E70) }; //updated
	WEAK symbol<int(void*/*const DObj*/ obj, int* partBits, uint16_t boneIndex, const vec3_t* trans, const vec3_t* angles)> DObjSetLocalTag{ OFFSET(0x14233E220) }; //updated

	/* DVAR */
	WEAK symbol<game::dvarStrHash_t(const char* str)> Dvar_GenerateHash{ OFFSET(0x14133DBF0) }; //updated
	WEAK symbol<bool(const dvar_t* dvar)> Dvar_GetBool{ OFFSET(0x1422BCED0) }; //updated
	WEAK symbol<const char* (const dvar_t* dvar)> Dvar_GetString{ OFFSET(0x1422BF590) }; //updated
	WEAK symbol<void(const char* dvarName, const char* string, bool createIfMissing)> Dvar_SetFromStringByName{ OFFSET(0x1422C7500) }; //updated
	// Variables
	WEAK symbol<char*> s_dvarPool{ OFFSET(0x157AC6220) }; //updated
	WEAK symbol<int> g_dvarCount{ OFFSET(0x157AC61CC) }; //updated

	/* LOBBYSESSION */
	WEAK symbol<int(INT64 lobbySession, LobbyClientType clientType)> LobbySession_GetClientCount{ OFFSET(0x141ECB030) }; //updated

	/* LUA / UI */
	WEAK symbol<const char* (const char* name)> UI_SafeTranslateString{ OFFSET(0x14228ED50) }; //updated
	WEAK symbol<game::LUIElement* (const char* rootName, lua::lua_State* luaVM)> UI_GetRootElement{ OFFSET(0x142704B00) }; //updated
	WEAK symbol<void(LUIScopedEvent* event, lua::lua_State* luaVM, const char* rootName, const char* eventName)> GetLUIScopedEvent{ OFFSET(0x1426FE790) }; //updated
	WEAK symbol<void(LUIScopedEvent* event)> ExecuteLUIScopedEvent{ OFFSET(0x1426FE9C0) }; //updated

	/* MATERIAL */
	WEAK symbol<INT64(const char* name, int imageTrack, bool errorIfMissing, int waitTime)> Material_RegisterHandle{ OFFSET(0x141CD4B90) }; //updated

	/* SYS */
	WEAK symbol<int()> Sys_IsDatabaseReady2{ OFFSET(0x142183A60) }; //updated
	WEAK symbol<void()> Sys_ShowConsole{ OFFSET(0x1423333C0) }; //updated
	WEAK symbol<bool()> Sys_IsMainThread{ OFFSET(0x142183A90) }; //updated
}
