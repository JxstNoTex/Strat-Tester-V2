#include <std_include.hpp>
#include "gsc_custome.hpp"
#include "gsc_funcs.hpp"
#include "game/game.hpp"
#include "loader/component_loader.hpp"
#include <utils/hook.hpp>

#define DETOUR_LOGGING

namespace gsc_custome
{


	

	namespace Opcodes
	{

		void VM_OP_GetLazyFunction(INT32 inst, INT64* fs_0, INT64 vmc, bool* terminate)
		{
			INT64 base = (*fs_0 + 3) & 0xFFFFFFFFFFFFFFFCLL;
			INT32 Namespace = *(INT32*)base;
			INT32 Function = *(INT32*)(base + 4);
			char* script = (char*)(*fs_0 + (*(INT32*)(base + 8)));
			auto asset = game::DB_FindXAssetHeader(game::XAssetType::ASSET_TYPE_SCRIPTPARSETREE,script, false, 0);

			if (!asset)
			{
				*(INT32*)(fs_0[1] + 0x18) = 0x0; // undefined
				fs_0[1] += 0x10; // change stack top
				*fs_0 = base + 0xC; // move past the data
				return;
			}

			auto buffer = *(char**)(asset + 0x10);
			auto exportsOffset = *(INT32*)(buffer + 0x20);
			auto exports = (INT64)(exportsOffset + buffer);
			auto numExports = *(INT16*)(buffer + 0x3A);
			__t7export* currentExport = (__t7export*)exports;
			bool found = false;

			for (INT16 i = 0; i < numExports; i++, currentExport++)
			{
				if (currentExport->funcName != Function)
				{
					continue;
				}
				if (currentExport->funcNS != Namespace)
				{
					continue;
				}
				found = true;
				break;
			}

			if (!found)
			{
				*(INT32*)(fs_0[1] + 0x18) = 0x0; // undefined
				fs_0[1] += 0x10; // change stack top
				*fs_0 = base + 0xC; // move past the data
				return;
			}

			*(INT32*)(fs_0[1] + 0x18) = 0xE; // assign the top variable's type
			*(INT64*)(fs_0[1] + 0x10) = (INT64)buffer + currentExport->bytecodeOffset; // assign the top variable's value
			fs_0[1] += 0x10; // change stack top
			*fs_0 = base + 0xC; // move past the data
		}

		void VM_OP_GetLocalFunction(INT32 inst, INT64* fs_0, INT64 vmc, bool* terminate)
		{
			INT64 base = (*fs_0 + 3) & 0xFFFFFFFFFFFFFFFCLL;
			INT32 jumpOffset = *(INT32*)base;
			*fs_0 = base + 0x4; // move past the data

			INT64 fnPtr = *fs_0 + jumpOffset;
			*(INT32*)(fs_0[1] + 0x18) = 0xE; // assign the top variable's type
			*(INT64*)(fs_0[1] + 0x10) = (INT64)fnPtr; // assign the top variable's value
			fs_0[1] += 0x10; // change stack top
		}

		void VM_OP_NOP(INT32 inst, INT64* fs_0, INT64 vmc, bool* terminate)
		{
		}

		#define OFF_ScrVm_Opcodes REBASE(0x1432E6350)
		void Init()
		{
			// Change Opcode Handler 0x16 to VM_OP_GetLazyFunction
			*(INT64*)(0x16 * 8 + OFF_ScrVm_Opcodes) = (INT64)VM_OP_GetLazyFunction;

			// Change Opcode Handler 0x17 to VM_OP_GetLocalFunction
			*(INT64*)(0x17 * 8 + OFF_ScrVm_Opcodes) = (INT64)VM_OP_GetLocalFunction;

			// Change Opcode Handler 0x1A to VM_OP_NOP
			*(INT64*)(0x1A * 8 + OFF_ScrVm_Opcodes) = (INT64)VM_OP_NOP;
		}
	}

	bool DetoursLinked{};

	void Register_GSIC(int DetourCount, INT64 ScriptOffset)
	{
		for (auto ReadDetour : loadedHeader)
		{
			ScriptDetour* detour = new ScriptDetour();
			detour->hFixup = ReadDetour->FixupOffset + ScriptOffset;
			detour->ReplaceFunction = ReadDetour->ReplaceFunction;
			detour->ReplaceNamespace = ReadDetour->ReplaceNamespace;
			detour->FixupSize = ReadDetour->FixupSize;
			memcpy(detour->ReplaceScriptName, ReadDetour->ReplaceScriptName, sizeof(detour->ReplaceScriptName));
			printf("Detour Parsed: {FixupName:%x, ReplaceNamespace:%x, ReplaceFunction:%x, FixupOffset:%x, FixupSize:%x} {FixupMin:%p, FixupMax:%p}\n", ReadDetour->FixupName, ReadDetour->ReplaceNamespace, ReadDetour->ReplaceFunction, ReadDetour->FixupOffset, ReadDetour->FixupSize, detour->hFixup, detour->hFixup + detour->FixupSize);
			RegisteredDetours.push_back(detour);
			DetoursLinked = false;
		}
	}

	namespace
	{
		
		bool DetoursReset{};
		
		bool DetoursEnabled = true;

		bool _IsBadReadPtr(void* p)
		{
			MEMORY_BASIC_INFORMATION mbi = { 0 };
			if (::VirtualQuery(p, &mbi, sizeof(mbi)))
			{
				DWORD mask = (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY);
				bool b = !(mbi.Protect & mask);
				// check the page is not a guard page
				if (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) b = true;

				return b;
			}
			return true;
		}

		void LinkDetours()
		{
			LinkedDetours.clear();
			for (auto it = RegisteredDetours.begin(); it != RegisteredDetours.end(); it++)
			{
					auto detour = *it;
					if (detour->ReplaceScriptName[0]) // not a builtin
					{
#ifdef DETOUR_LOGGING
						printf("Linking replacement %x<%s>::%x...", detour->ReplaceNamespace, detour->ReplaceScriptName, detour->ReplaceFunction);
#endif
						// locate the script to replace
						auto asset = game::DB_FindXAssetHeader(game::XAssetType::ASSET_TYPE_SCRIPTPARSETREE,detour->ReplaceScriptName, false, 0);
						if (!asset)
						{
#ifdef DETOUR_LOGGING
							printf("Failed to locate %s...", detour->ReplaceScriptName);
#endif
							continue;
						}

#ifdef DETOUR_LOGGING
						printf("Located xAssetHeader...");
#endif
						// locate the target export to link
						auto buffer = *(char**)(asset + 0x10);
						auto exportsOffset = *(INT32*)(buffer + 0x20);
						auto exports = (INT64)(exportsOffset + buffer);
						auto numExports = *(INT16*)(buffer + 0x3A);
						__t7export* currentExport = (__t7export*)exports;
						for (INT16 i = 0; i < numExports; i++, currentExport++)
						{
							if (currentExport->funcName != detour->ReplaceFunction)
							{
								continue;
							}
							if (currentExport->funcNS != detour->ReplaceNamespace)
							{
								continue;
							}
#ifdef DETOUR_LOGGING
							printf("Found export at %p!", (INT64)buffer + currentExport->bytecodeOffset);
#endif
							LinkedDetours[(INT64)buffer + currentExport->bytecodeOffset] = detour;
							break;
						}
					}
					else
					{
#ifdef DETOUR_LOGGING
						printf("Linking replacement for builtin %x...", detour->ReplaceFunction);
#endif
						INT32 discardType;
						INT32 discardMinParams;
						INT32 discardMaxParams;
						auto hReplace = game::Scr_GetFunction(detour->ReplaceFunction, &discardType, &discardMinParams, &discardMaxParams);
						if (!hReplace)
						{
							hReplace = game::Scr_GetMethod(detour->ReplaceFunction, &discardType, &discardMinParams, &discardMaxParams);
						}
						if (hReplace)
						{
#ifdef DETOUR_LOGGING
							printf("Found function definition at %p!", hReplace);
#endif
							LinkedDetours[hReplace] = detour;
						}
					}
				}
			
			DetoursLinked = true;
		}

		void ResetDetours()
		{
#ifdef DETOUR_LOGGING
			printf("Resetting detours...");
#endif
			for (auto it = AppliedFixups.begin(); it != AppliedFixups.end(); it++)
			{
				if (_IsBadReadPtr(it->first))
				{
					continue;
				}
				*it->first = it->second;
			}
			AppliedFixups.clear();
			DetoursReset = true;
			DetoursLinked = false;
			//DetoursEnabled = false;
		}


		bool CheckDetour(INT32 inst, INT64* fs_0, INT32 offset)
		{
			
			// detours are not supported in UI level
			if (game::Com_IsRunningUILevel)
			{
				if (!DetoursReset)
				{
					ResetDetours();
				}
				return false;
			}
			if (inst)
			{
				// csc is not supported at this time
				return false;
			}
			printf("checking Detours\n");
			bool fixupApplied = false;
			if (!DetoursLinked)
			{
				printf("Linking Detours\n");
				LinkDetours();
			}
			INT64 ptrval = *(INT64*)((*fs_0 + 7 + offset) & 0xFFFFFFFFFFFFFFF8);
			if (LinkedDetours.find(ptrval) != LinkedDetours.end() && LinkedDetours[ptrval]->hFixup)
			{
				INT64 fs_pos = *fs_0;
				// if pointer is below fixup or above it, the pointer is not within the detour and thus can be fixed up
				if (LinkedDetours[ptrval]->hFixup > fs_pos || ((LinkedDetours[ptrval]->hFixup + LinkedDetours[ptrval]->FixupSize) <= fs_pos))
				{
#ifdef DETOUR_LOGGING
					printf("Replaced call at %p to fixup %p! Opcode: %x", (INT64)((*fs_0 + 7 + offset) & 0xFFFFFFFFFFFFFFF8), LinkedDetours[ptrval]->hFixup, *(INT16*)(*fs_0 - 2));
#endif
					AppliedFixups[(INT64*)((*fs_0 + 7 + offset) & 0xFFFFFFFFFFFFFFF8)] = ptrval;
					*(INT64*)((*fs_0 + 7 + offset) & 0xFFFFFFFFFFFFFFF8) = LinkedDetours[ptrval]->hFixup;
					DetoursReset = false;
					fixupApplied = true;
				}
			}
			return fixupApplied;
		}


		void VM_OP_GetFunction(INT32 inst, INT64* fs_0, INT64 vmc, bool* terminate)
		{
			CheckDetour(inst, fs_0, NULL);
			VM_OP_GetFunction_Old(inst, fs_0, vmc, terminate);
		}

		void VM_OP_GetAPIFunction(INT32 inst, INT64* fs_0, INT64 vmc, bool* terminate)
		{
			CheckDetour(inst, fs_0, NULL);
			VM_OP_GetAPIFunction_Old(inst, fs_0, vmc, terminate);
		}

		void VM_OP_ScriptFunctionCall(INT32 inst, INT64* fs_0, INT64 vmc, bool* terminate)
		{
			CheckDetour(inst, fs_0, 1);
			VM_OP_ScriptFunctionCall_Old(inst, fs_0, vmc, terminate);
		}

		void VM_OP_ScriptMethodCall(INT32 inst, INT64* fs_0, INT64 vmc, bool* terminate)
		{
			CheckDetour(inst, fs_0, 1);
			VM_OP_ScriptMethodCall_Old(inst, fs_0, vmc, terminate);
		}

		void VM_OP_ScriptThreadCall(INT32 inst, INT64* fs_0, INT64 vmc, bool* terminate)
		{
			CheckDetour(inst, fs_0, 1);
			VM_OP_ScriptThreadCall_Old(inst, fs_0, vmc, terminate);
		}

		void VM_OP_ScriptMethodThreadCall(INT32 inst, INT64* fs_0, INT64 vmc, bool* terminate)
		{
			CheckDetour(inst, fs_0, 1);
			VM_OP_ScriptMethodThreadCall_Old(inst, fs_0, vmc, terminate);
		}

		void VM_OP_CallBuiltin(INT32 inst, INT64* fs_0, INT64 vmc, bool* terminate)
		{
			if (CheckDetour(inst, fs_0, 1))
			{
				// spoof opcode to ScriptFunctionCall (because we are no longer calling a builtin)
				*(INT16*)(*fs_0 - 2) = 0x203;
				VM_OP_ScriptFunctionCall_Old(inst, fs_0, vmc, terminate);
				return;
			}
			VM_OP_CallBuiltin_Old(inst, fs_0, vmc, terminate);
		}

		void VM_OP_CallBuiltinMethod(INT32 inst, INT64* fs_0, INT64 vmc, bool* terminate)
		{
			if (CheckDetour(inst, fs_0, 1))
			{
				// spoof opcode to ScriptMethodCall (because we are no longer calling a builtin)
				*(INT16*)(*fs_0 - 2) = 0x207;
				VM_OP_ScriptMethodCall_Old(inst, fs_0, vmc, terminate);
				return;
			}
			VM_OP_CallBuiltinMethod_Old(inst, fs_0, vmc, terminate);
		}
	}


#define OFF_ScrVm_Opcodes REBASE(0x1432E6350)
#define OFF_ScrVm_Opcodes2 REBASE(0x143306350)
	void VTableReplace(INT64 sub_offset, tVM_Opcode ReplaceFunc, tVM_Opcode* OutOld)
	{
		
		INT64 stub_final = REBASE(sub_offset);

		printf("replacing VTable: %x\n", stub_final);

		INT64 handler_table = OFF_ScrVm_Opcodes;
		*OutOld = (tVM_Opcode)stub_final;
		for (int i = 0; i < 0x2000; i++)
		{
			if (*(INT64*)(handler_table + (i * 8)) == stub_final)
			{
				*(INT64*)(handler_table + (i * 8)) = (INT64)ReplaceFunc;
			}
		}

		handler_table = OFF_ScrVm_Opcodes2;

		for (int i = 0; i < 0x2000; i++)
		{
			if (*(INT64*)(handler_table + (i * 8)) == stub_final)
			{
				*(INT64*)(handler_table + (i * 8)) = (INT64)ReplaceFunc;
			}
		}
	}

	void InstallHooks()
	{
		// opcodes to hook:
		VTableReplace((0x1412D0890), VM_OP_GetFunction, &VM_OP_GetFunction_Old);
		VTableReplace((0x1412D0A30), VM_OP_GetAPIFunction, &VM_OP_GetAPIFunction_Old);
		VTableReplace((0x1412CEE80), VM_OP_ScriptFunctionCall, &VM_OP_ScriptFunctionCall_Old);
		VTableReplace((0x1412CF1D0), VM_OP_ScriptMethodCall, &VM_OP_ScriptMethodCall_Old);
		VTableReplace((0x1412CFB10), VM_OP_ScriptThreadCall, &VM_OP_ScriptThreadCall_Old);
		VTableReplace((0x1412CF570), VM_OP_ScriptMethodThreadCall, &VM_OP_ScriptMethodThreadCall_Old);
		VTableReplace((0x1412CE460), VM_OP_CallBuiltin, &VM_OP_CallBuiltin_Old);
		VTableReplace((0x1412CE3A0), VM_OP_CallBuiltinMethod, &VM_OP_CallBuiltinMethod_Old);
	}

	



	class component final : public component_interface
	{
	public:
		void post_start() override
		{
			Opcodes::Init();
			InstallHooks();
		}

		void start_hooks() override
		{

		}

		void destroy_hooks() override
		{

		}
	};
}

REGISTER_COMPONENT(gsc_custome::component)