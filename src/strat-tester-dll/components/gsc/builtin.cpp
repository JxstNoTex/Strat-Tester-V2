#include "std_include.hpp"
#include "loader/component_loader.hpp"
#include "game/game.hpp"

namespace gsc
{
	namespace builtin
	{
		std::unordered_map<int, void*> builtinFunc;
		void isProfileBuildInterval(game::scriptInstance_t inst)
		{
			printf("isProfileBuildInterval was called\n");
			auto funcHash = game::Scr_GetInt(inst, 0);
			
			if (funcHash == fnv1a("detour"))
			{
				printf("Detour was called\n");
			}

			if (builtinFunc.find(funcHash) == builtinFunc.end())
			{
				printf("Unknown Build-In Function Called: %x\n", funcHash);
				return;
			}

			reinterpret_cast<void(__fastcall*)(int)>(builtinFunc[funcHash])(inst);
		}

		void RegisterFunc(const char* name, void* ptr)
		{
			builtinFunc[fnv1a(name)] = ptr;
		}


		class component final : public component_interface
		{
		public:
			void post_start() override
			{
				game::isProfileBuildFunctionDef->max_args = 255;
				game::isProfileBuildFunctionDef->actionFunc = isProfileBuildInterval;
			}

			void pre_destroy() override
			{

			}
		};
	}
}

REGISTER_COMPONENT(gsc::builtin::component)