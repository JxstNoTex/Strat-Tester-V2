#pragma once


namespace gsc
{
	struct __t7export
	{
		INT32 discardCRC32;
		INT32 bytecodeOffset;
		INT32 funcName;
		INT32 funcNS;
		INT32 discardParamsFlagsPad;
	};

	struct ScriptDetour
	{
		char ReplaceScriptName[256];
		INT32 ReplaceNamespace;
		INT32 ReplaceFunction;
		INT64 hFixup;
		INT32 FixupSize;
	};


	struct ReadScriptDetour
	{
		INT32 FixupName;
		INT32 ReplaceNamespace;
		INT32 ReplaceFunction;
		INT32 FixupOffset;
		INT32 FixupSize;
		char ReplaceScriptName[256];
	};

	constexpr const char* gsic_magic = "GSIC";

	constexpr const uint64_t gsc_magic = 0x1C000A0D43534780;

	

	namespace opcodes
	{

	}

	namespace detour
	{
		static bool DetoursReset{};
		typedef void(__fastcall* tVM_Opcode)(INT32 inst, INT64* fs_0, INT64 vmc, bool* terminate);
		static tVM_Opcode VM_OP_GetFunction_Old = NULL;
		static tVM_Opcode VM_OP_GetAPIFunction_Old = NULL;
		static tVM_Opcode VM_OP_ScriptFunctionCall_Old = NULL;
		static tVM_Opcode VM_OP_ScriptMethodCall_Old = NULL;
		static tVM_Opcode VM_OP_ScriptThreadCall_Old = NULL;
		static tVM_Opcode VM_OP_ScriptMethodThreadCall_Old = NULL;
		static tVM_Opcode VM_OP_CallBuiltin_Old = NULL;
		static tVM_Opcode VM_OP_CallBuiltinMethod_Old = NULL;
		static std::vector<ScriptDetour> RegisteredDetours;
		static std::unordered_map<INT64, ScriptDetour> LinkedDetours;
		static std::unordered_map<INT64*, INT64> AppliedFixups;
		static bool DetoursEnabled = true; //change to {} after debugging
		static bool DetoursLinked = false;
		static std::vector<ReadScriptDetour> LoadedGSIC;
		void LinkDetours();
		void RegisterDetour(void* data, uint64_t offset);
	}
}