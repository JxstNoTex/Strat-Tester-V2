#include "std_include.hpp"
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include "detours.h"
#include "builtins.h"

namespace gsc
{
	namespace funcs
	{
		void GScr_erasefunc(game::scriptInstance_t scriptInst)
		{
			char* str_file = (char*)game::Scr_GetString(scriptInst, 1);
			int n_namespace = game::Scr_GetInt(scriptInst, 2);
			int n_func = game::Scr_GetInt(scriptInst, 3);

			if (!str_file || !n_namespace || !n_func)
			{
				return; // bad inputs
			}

			auto asset = game::DB_FindXAssetHeader(game::XAssetType::ASSET_TYPE_SCRIPTPARSETREE, str_file, false, 0);
			if (!asset)
			{
				return; // couldn't find asset, quit
			}
			auto buffer = *(char**)(asset + 0x10);

			if (!buffer)
			{
				return; // buffer doesnt exist
			}

			auto exportsOffset = *(INT32*)(buffer + 0x20);
			auto exports = (INT64)(exportsOffset + buffer);
			auto numExports = *(INT16*)(buffer + 0x3A);
			__t7export* currentExport = (__t7export*)exports;
			bool b_found = false;
			for (INT16 i = 0; i < numExports; i++, currentExport++)
			{
				if (currentExport->funcName != n_func)
				{
					continue;
				}
				if (currentExport->funcNS != n_namespace)
				{
					continue;
				}
				b_found = true;
				break;
			}

			if (!b_found)
			{
				return; // couldnt find the function
			}

			INT32 target = currentExport->bytecodeOffset;
			char* fPos = buffer + currentExport->bytecodeOffset;

			b_found = false;
			currentExport = (__t7export*)exports;
			__t7export* lowest = NULL;
			for (INT16 i = 0; i < numExports; i++, currentExport++)
			{
				if (currentExport->bytecodeOffset <= target)
				{
					continue;
				}
				if (!lowest || (currentExport->bytecodeOffset < lowest->bytecodeOffset))
				{
					lowest = currentExport;
					b_found = true;
				}
			}

			// dont erase prologue

			auto code = *(UINT16*)fPos;

			if (code == 0xD || code == 0x200D) // CheckClearParams
			{
				fPos += 2;
			}
			else
			{
				fPos += 2;
				BYTE numParams = *(BYTE*)fPos;
				fPos += 2;
				for (BYTE i = 0; i < numParams; i++)
				{
					fPos = (char*)((INT64)fPos + 3 & 0xFFFFFFFFFFFFFFFCLL) + 4;
					fPos += 1; // type
				}
				if ((INT64)fPos & 1)
				{
					fPos++;
				}
			}

			char* fStart = fPos;
			char* fEnd = b_found ? (lowest->bytecodeOffset + buffer) : (fStart + 2); // cant erase entire functions if we dont know the end

			while (fStart < fEnd)
			{
				*(UINT16*)fStart = 0x10; // OP_END
				fStart += 2;
			}


		}

		void DetourScriptCall(game::scriptInstance_t inst)
		{
			if (inst)
			{
				return;
			}

			detour::DetoursEnabled = true;
			detour::LinkDetours();
		}

		void RelinkDetours(game::scriptInstance_t inst)
		{
			if (inst)
			{
				return;
			}
			detour::LinkDetours();
		}

		class component final : public component_interface
		{
			void post_start() override
			{
				builtin::RegisterFunc("relinkdetours", RelinkDetours);
				builtin::RegisterFunc("detour", DetourScriptCall);
				builtin::RegisterFunc("erasefunc", GScr_erasefunc);
			}
		};

	}
}