#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "havok/hks_api.hpp"
#include "havok/lua_api.hpp"

#include <utils/hook.hpp>
#include "gsc_custome.hpp"
#include <utils/io.hpp>
namespace gsc_loading
{
	namespace
	{
		constexpr const char* gsic_magic = "GSIC";

		constexpr const uint64_t gsc_magic = 0x1C000A0D43534780;

		

		struct scriptparsetree
		{
			game::SPT_Header header{};
			std::string data{};
			size_t gsic_header_size{};
			gsc_custome::ScriptDetour Detour;
			int count;
			bool can_read_gsic(size_t bytes)
			{
				return data.length() >= gsic_header_size + bytes;
			}

			bool load_gsic()
			{
				byte* ptr = (byte*)data.data();

				if (!can_read_gsic(4) || memcmp(gsic_magic, ptr, 4))
				{
					return true;
				}
				gsic_header_size += 4;

				if (!can_read_gsic(4))
				{
					printf("can't read gsic fields");
					return false;
				}

				int32_t fields = *reinterpret_cast<int32_t*>(ptr + gsic_header_size);
				gsic_header_size += 4;

				printf("Found %d Fields\n", fields);

				for (size_t i = 0; i < fields; i++)
				{
					if (!can_read_gsic(4))
					{
						printf("can't read gsic field type\n");
						return false;
					}

					int32_t field_type = *reinterpret_cast<int32_t*>(ptr + gsic_header_size);
					gsic_header_size += 4;
					printf("Found Field Type: %d\n", field_type);

					if (!can_read_gsic(4))
					{
						printf("can't read gsic detours count");
						return false;
					}
					int32_t detour_count = *reinterpret_cast<int32_t*>(ptr + gsic_header_size);
					gsic_header_size += 4;
					printf("Detour Count: %d\n", detour_count);
					if (!can_read_gsic(detour_count * 256ull))
					{
						printf("can't read detours\n");
						return false;
					}

					for (int i = 0; i < detour_count; i++)
					{
						gsc_custome::ReadScriptDetour* detour;
						detour->FixupName = *reinterpret_cast<uint32_t*>(ptr + gsic_header_size + 4);
						detour->ReplaceFunction = *reinterpret_cast<uint32_t*>(ptr + gsic_header_size + 4);
						detour->ReplaceNamespace = *reinterpret_cast<uint32_t*>(ptr + gsic_header_size + 8);
						detour->FixupOffset = *reinterpret_cast<uint32_t*>(ptr + gsic_header_size + 12);
						detour->FixupSize = *reinterpret_cast<uint32_t*>(ptr + gsic_header_size + 16);
						memcpy_s(detour->ReplaceScriptName, sizeof(detour->ReplaceScriptName), ptr + gsic_header_size + sizeof(detour) - 256, sizeof(detour->ReplaceScriptName));

						printf("read Detour: %d : namespace_%X<%s>::function_%x  / offset= %x+%X\n", detour_count, detour->ReplaceNamespace, detour->ReplaceScriptName, detour->ReplaceFunction, detour->FixupOffset, detour->FixupSize);
						gsc_custome::loadedHeader.emplace_back(detour);
						gsic_header_size += 256;
						count = detour_count;
					}

				}
				data = data.substr(gsic_header_size, data.length() - gsic_header_size);
				//memcpy(data.data(), data.data() + gsic_header_size, sizeof(data.data()) - gsic_header_size);
				return true;
			}


		};




	}

	void load_script()
	{
		scriptparsetree tmp{};

		std::string path = ".\\mods\\strat_tester_2.0\\zone\\scripts";

		for (const auto entry : utils::io::list_files(path))
		{
			if (utils::io::read_file(entry, &tmp.data))

				if (!tmp.load_gsic())
				{
					printf("error when reading GSIC header of %s", entry.c_str());
					
				}

			if (tmp.count)
			{
				printf("loaded %d detours\n", tmp.count);

			}

			if (tmp.data.length() < sizeof(char*) || *reinterpret_cast<uint64_t*>(tmp.data.data()) != gsc_magic)
			{
				printf("bad scriptparsetree magic: %d\n", *reinterpret_cast<uint64_t*>(tmp.data.data()));
				return;
			}

			

			auto header = (game::SPT_Header*)game::DB_FindXAssetHeader(game::XAssetType::ASSET_TYPE_SCRIPTPARSETREE, (char*)"scripts/shared/duplicaterender_mgr.gsc", false, 0);
			printf("current header info:\n name: %s\n size: %d\n buffer ptr: %x\n", (char*)header->name, header->size, header->buffer);


			//replace script
			header->size = tmp.data.size();
			char* script_obj = (char*)malloc(header->size);

			memcpy(script_obj, tmp.data.data(), header->size);
			memcpy((script_obj + 0x8), (header->buffer + 0x8), sizeof(int));

			header->buffer = script_obj;
			printf("new header size: %d\n new buffer address: %x\n", header->size, header->buffer);

			//gsc_custome::Register_GSIC(tmp.gsic);
			gsc_custome::Register_GSIC(tmp.count, (INT64)header->buffer);
		}
	}


	
	class component final : public component_interface
	{
	public:
		void post_start() override
		{
			load_script();
		}

		void start_hooks() override
		{
			
		}

		void destroy_hooks() override
		{

		}
	};

}
REGISTER_COMPONENT(gsc_loading::component)