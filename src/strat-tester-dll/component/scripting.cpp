#include <std_include.hpp>
#include "game/game.hpp"
#include "loader/component_loader.hpp"
#include "scripting.hpp"
#include <utils/string.hpp>
#include <utils/hook.hpp>
#include <utils/memory.hpp>
#include "console.hpp"
#include "resource.h"
#include <utils/nt.hpp>
#include "havok/lua_api.hpp"
#include "havok/hks_api.hpp"
#include "detours.h"
/*
namespace scripting
{
	static std::unordered_map<int, void(__fastcall*)(game::scriptInstance_t)> custom_functions;
	std::unordered_map<std::string, game::RawFile*> loaded_scripts;
	utils::hook::detour db_find_x_asset_header_hook;
	utils::hook::detour gscr_get_bgb_remaining_hook;
	utils::memory::allocator allocator;
	HMODULE hm;
	std::string Name = "scripts/shared/duplicaterender_mgr.gsc";
	int count;
	void* gsicdata;



	game::RawFile* get_loaded_scripts(const std::string& name)
	{
		const auto itr = loaded_scripts.find(name);
		return (itr == loaded_scripts.end()) ? nullptr : itr->second;
	}

	void print_loading_script(const std::string& name)
	{
		printf("********************** GSC Script **********************\n");
		printf("			Loading Script: %s\n", name.c_str());
		printf("********************************************************\n");
	}


	game::RawFile* db_find_x_asset_header_stub(const game::XAssetType type, const char* name,
		const bool error_if_missing,
		const int wait_time)
	{
		auto* asset_header = db_find_x_asset_header_hook.invoke<game::RawFile*>(type, name, error_if_missing, wait_time);

		if (type != game::ASSET_TYPE_SCRIPTPARSETREE)
		{
			return asset_header;
		}

		auto* script = get_loaded_scripts(name);
		//printf("Trying to load script: %s", name);
		if (script)
		{
			RegisterDetours(gsicdata, count, (char*)script->buffer);
			return script;
		}

		return asset_header;
	}

	
	std::string gsiData;

	void is_profile_build(game::scriptInstance_t inst)
	{
		//args start at index 1
		auto canonId = game::Scr_GetInt(inst, 1);
		if (custom_functions.find(canonId) == custom_functions.end())
		{
			printf("************** GSC script execution error **************\n");
			printf("		Unknown built-in function: %h", canonId);
			printf("********************************************************\n");
			return;
		}
		custom_functions[canonId](inst);
	}

	int server_script_checksum_stub()
	{
		return 1;
	}

	int load_script()
	{

		game::isProfileBuildFunctionDef->max_args = 255;
		game::isProfileBuildFunctionDef->actionFunc = is_profile_build;
		
		//gscr_get_bgb_remaining_hook.create(0x141A8CAB0_g, scr_loot_get_item_quantity_stub);
		utils::hook::call(0x1408F2E5D_g, server_script_checksum_stub);

		if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)&scripting::load_script, &hm) == 0)
		{
			printf("GetModuleHandleEx Failed");
			return 1;
		}	
		

		auto res = utils::nt::load_resource(GSIC);
		printf("resource size: %d\n", res.length());

		// Parse the resource
		uint8_t* bytePtr = reinterpret_cast<uint8_t*>(res.data());

		count = *reinterpret_cast<int*>(bytePtr);
		bytePtr += sizeof(int);

		int data1size = *reinterpret_cast<int*>(bytePtr);
		bytePtr += sizeof(int);

		int data2size = *reinterpret_cast<int*>(bytePtr);
		bytePtr += sizeof(int);

		std::string data1(reinterpret_cast<char*>(bytePtr), data1size);
		bytePtr += data1size;

		std::string data2(reinterpret_cast<char*>(bytePtr), data2size);
		bytePtr += data2size;

		auto* raw_file = allocator.allocate<game::RawFile>();

		raw_file->name = allocator.duplicate_string(Name);
		raw_file->buffer = allocator.duplicate_string(data2);
		raw_file->len = static_cast<int>(data2.length());

		print_loading_script(Name);

		loaded_scripts[Name] = raw_file;
		printf("count: %d\n", count);
		printf("gsi size: %d\n", data1size);
		printf("gscc data: %d\n", data2size);

		gsicdata = reinterpret_cast<void*>(data1.data());
		
		db_find_x_asset_header_hook.create(0x141420ED0_g, db_find_x_asset_header_stub);

		return 0;
	}

	void register_function(const char* name, void(*funcPtr)(game::scriptInstance_t inst))
	{
		custom_functions[fnv1a(name)] = funcPtr;
	}





	*//*void scr_loot_get_item_quantity_stub([[maybe_unused]] game::scriptInstance_t inst,
		[[maybe_unused]] game::scr_entref_t entref)
	{
		game::Scr_AddInt(game::SCRIPTINSTANCE_SERVER, 255);
	}*//*

	class component final : public component_interface
	{
	public:


		void start_hooks() override
		{

		}

		void destroy_hooks() override
		{
			db_find_x_asset_header_hook.disable();
		}
	};
}

REGISTER_COMPONENT(scripting::component)*/