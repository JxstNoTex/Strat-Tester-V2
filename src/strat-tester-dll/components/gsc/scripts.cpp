#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include "detours.h"
#include "builtins.h"
#include "utils/io.hpp"
#include "utils/hook.hpp"
#include "utils/memory.hpp"


namespace gsc
{
	namespace loading
	{
		std::string path = ".\\mods\\strat_tester_2.0\\zone\\scripts";
		utils::memory::allocator allocator;
		std::unordered_map<std::string, game::RawFile*> loadedScripts;
		utils::hook::detour db_FindXAssetHeader;

		game::RawFile* get_loaded_scripts(const std::string& name)
		{
			auto it = loadedScripts.find(name);
			if (it != loadedScripts.end())
			{
				return it->second;
			}
			return NULL;
		}

		game::RawFile* db_FindXAssetHeader_stub(const game::XAssetType type, const char* name,
			const bool error_if_missing,
			const int wait_time)
		{
			auto* assetHeader = db_FindXAssetHeader.invoke<game::RawFile*>(type, name, error_if_missing, wait_time);

			if (type != game::ASSET_TYPE_SCRIPTPARSETREE)
				return assetHeader;

			auto* script = get_loaded_scripts(name);

			if (script)
			{
				return script;
			}
			return assetHeader;
		}

		struct GSCIFile
		{
			std::string data;
			size_t count{};
			size_t gsic_header_size{};

			bool isValidGSIC(size_t bytes)
			{
				return data.length() >= gsic_header_size + bytes;
			}

			bool loadGSIC()
			{
				byte* ptr = (byte*)data.data();

				if (!isValidGSIC(4) || memcmp(gsic_magic, ptr, 4))
				{
					return true;
				}

				gsic_header_size += 4;

				if (!isValidGSIC(4))
				{
					printf("can't read gsic fields\n");
					return false;
				}

				int32_t fields = *reinterpret_cast<int32_t*>(ptr + gsic_header_size);
				gsic_header_size += 4;

#ifdef DEBUG
				printf("Found %d Fields\n", fields);
#endif // DEBUG

				for (size_t i = 0; i < fields; i++)
				{
					if (!isValidGSIC(4))
					{
						printf("can't read gsic field type\n");
						return false;
					}

					int32_t field_type = *reinterpret_cast<int32_t*>(ptr + gsic_header_size);
					gsic_header_size += 4;
#ifdef DEBUG
					printf("Found Field Type: %d\n", field_type);
#endif // DEBUG
					if (!isValidGSIC(4))
					{
						printf("can't read gsic detours count\n");
						return false;
					}

					int32_t detour_count = *reinterpret_cast<int32_t*>(ptr + gsic_header_size);
					gsic_header_size += 4;
#ifdef DEBUG
					printf("Detour Count: %d\n", detour_count);
#endif // DEBUG
					//this is the last sanity check we do after this point we can assume that the gsic data is valid
					if (!isValidGSIC(detour_count * 256ull))
					{
						printf("can't read detours\n");
						return false;
					}

					for (int i = 0; i < detour_count; i++)
					{
						gsc::ReadScriptDetour detour;
						detour.FixupName = *reinterpret_cast<uint32_t*>(ptr + gsic_header_size + 0);
						detour.ReplaceNamespace = *reinterpret_cast<uint32_t*>(ptr + gsic_header_size + 4);
						detour.ReplaceFunction = *reinterpret_cast<uint32_t*>(ptr + gsic_header_size + 8);
						detour.FixupOffset = *reinterpret_cast<uint32_t*>(ptr + gsic_header_size + 12);
						detour.FixupSize = *reinterpret_cast<uint32_t*>(ptr + gsic_header_size + 16);
						memcpy_s(detour.ReplaceScriptName, 256, ptr + gsic_header_size + sizeof(detour) - 256, 256);
						detour::LoadedGSIC.push_back(detour);
						gsic_header_size += 256;
#ifdef DEBUG
						printf("loaded Detour %d : FixUpName: %x, namespace_%x<%s>function_%x || offset: %x + %x\n", detour_count, detour.FixupName, detour.ReplaceNamespace, detour.ReplaceScriptName, detour.ReplaceFunction, detour.FixupOffset, detour.FixupSize);
						printf("Loaded gsics %d\n", detour::LoadedGSIC.size());
#endif // DEBUG
						
					}
				}
				data = data.substr(gsic_header_size, data.length() - gsic_header_size);
				return true;
			}
		};

		void LoadScriptFolder()
		{
			for (const auto entry : utils::io::list_files(path))
			{
				GSCIFile temp{};
				if (!utils::io::read_file(entry, &temp.data))
				{
					printf("Error While Loading GCS File %s", entry.c_str());
				}

				if (!temp.loadGSIC())
				{
					printf("error when reading GSIC header of %s", entry.generic_string());
				}

				if (temp.data.length() < sizeof(char*) || *reinterpret_cast<uint64_t*>(temp.data.data()) != gsc_magic)
				{
					printf("bad scriptparsetree magic: %d\n", *reinterpret_cast<uint64_t*>(temp.data.data()));
					return;
				}

				auto* rawFile = allocator.allocate<game::RawFile>();
				//dont give a shit rn we hard code this bitch dynamic loading comes later
				rawFile->name = allocator.duplicate_string("scripts/shared/duplicaterender_mgr.gsc");
				rawFile->buffer = allocator.duplicate_string(temp.data);
				rawFile->len = static_cast<int>(temp.data.length());

				loadedScripts["scripts/shared/duplicaterender_mgr.gsc"] = rawFile;
				for (auto it = detour::LoadedGSIC.begin(); it != detour::LoadedGSIC.end(); ++it)
				{
					ReadScriptDetour detour = *it;
					printf("FixUpName: %x, namespace_%x<%s>function_%x || offset: %x + %x\n", detour.FixupName, detour.ReplaceNamespace, detour.ReplaceScriptName, detour.ReplaceFunction, detour.FixupOffset, detour.FixupSize);
					game::RawFile* header = game::DB_FindXAssetHeader(game::XAssetType::ASSET_TYPE_SCRIPTPARSETREE, (char*)"scripts/shared/duplicaterender_mgr.gsc", false, 0);
					printf("cur buffer %x\n", (uint64_t)header->buffer);
					detour::RegisterDetour(&detour, (uint64_t)header->buffer);
				}

			}
		}



		int ScriptChecksumStub()
		{
			return 1;
		}

		void clear_script_memory()
		{
			loadedScripts.clear();
			allocator.clear();
		}

		class component final : public component_interface
		{
		public:
			void post_start() override
			{
				LoadScriptFolder();
			}
			void Start_frontend_hooks() override
			{
				db_FindXAssetHeader.create(0x141420ED0_g, db_FindXAssetHeader_stub);
				utils::hook::call(0x1408F2E5D_g, ScriptChecksumStub);
			}
			void pre_destroy() override
			{
				//move this to game::event::_on_shotdown_game later
				clear_script_memory();
			}
			void db_destroy_hooks2() override
			{
				db_FindXAssetHeader.clear();
			}
		};
	}
}

REGISTER_COMPONENT(gsc::loading::component)