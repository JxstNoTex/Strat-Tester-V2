#include <std_include.hpp>
#include "game/game.hpp"
#include "loader/component_loader.hpp"
#include "utils/string.hpp"
#include "utils/hook.hpp"
#include "arxan.hpp"

namespace arxan
{
	char* ModuleBase = (char*)GetModuleHandle(NULL);
#define EXCP_HEAT_PERCENT (ModuleBase + 0x129A2C7)
#define EXCP_HEAT_RESUME  (ModuleBase + 0x129A2F2)
	typedef unsigned long long(__fastcall* ZwContinue_t)(PCONTEXT ThreadContext, BOOLEAN RaiseAlert);
	static ZwContinue_t ZwContinue = reinterpret_cast<ZwContinue_t>(GetProcAddress(GetModuleHandleA("ntdll.dll"), "ZwContinue"));
	struct integrity_handler_context
	{
		uint32_t* computed_checksum;
		uint32_t* original_checksum;
	};

	void ExceptionDispatcher(PEXCEPTION_RECORD ExceptionRecord, PCONTEXT ContextRecord)
	{
		if (ExceptionRecord->ExceptionAddress == EXCP_HEAT_PERCENT)
		{
			ContextRecord->Xmm1 = (M128A)0;					// Set the heat percent multiplier to 0
			ContextRecord->Rip = (DWORD64)EXCP_HEAT_RESUME; // jump to the next instruction
			ZwContinue(ContextRecord, false);
		}
	}

	void InstallExceptionDispatcher()
	{
		auto ntdllModule = GetModuleHandleA("ntdll.dll");
		if (!ntdllModule) {
			return;
		}
		auto dispatcherptr = (char*)GetProcAddress(ntdllModule, "KiUserExceptionDispatcher");
		auto distance = *(int*)(dispatcherptr + 4); // LdrParentRtlInitializeNtUserPfn
		auto ptr = (dispatcherptr + 8) + distance;

		auto OldProtection = 0ul;
		VirtualProtect(reinterpret_cast<void*>(ptr), 8, PAGE_EXECUTE_READWRITE, &OldProtection);
		*reinterpret_cast<void**>(ptr) = (void*)ExceptionDispatcher;
		VirtualProtect(reinterpret_cast<void*>(ptr), 8, OldProtection, &OldProtection);
	}

	bool is_handler_context(uint8_t* stack_frame, const uint32_t computed_checksum, const uint32_t frame_offset)
	{
		auto* potential_address = *reinterpret_cast<uint32_t**>(stack_frame + frame_offset);

		int64_t diff = reinterpret_cast<uint64_t>(stack_frame) - reinterpret_cast<uint64_t>(potential_address);
		diff = std::abs(diff);

		return diff < 0x1000 && *potential_address == computed_checksum;
	}

	integrity_handler_context* search_handler_context(uint8_t* stack_frame, const uint32_t computed_checksum)
	{
		for (uint32_t frame_offset = 0x38; frame_offset < 0x90; frame_offset += 8)
		{
			if (is_handler_context(stack_frame, computed_checksum, frame_offset))
			{
				return reinterpret_cast<integrity_handler_context*>(stack_frame + frame_offset);
			}
		}

		return nullptr;
	}

	uint32_t adjust_integrity_checksum(const uint64_t return_address, uint8_t* stack_frame,
		const uint32_t current_checksum)
	{
		//const auto handler_address = return_address - 5;
		const auto* context = search_handler_context(stack_frame, current_checksum);

		if (!context)
		{
			OutputDebugStringA(utils::string::va("Unable to find frame offset for: %llX", return_address));
			return current_checksum;
		}

		const auto correct_checksum = *context->original_checksum;
		*context->computed_checksum = correct_checksum;

		/*if (current_checksum != correct_checksum)
		{
			OutputDebugStringA(utils::string::va("Adjusting checksum (%llX): %X -> %X", handler_address,
												 current_checksum, correct_checksum));
		}*/

		return correct_checksum;
	}

	void patch_intact_basic_block_integrity_check(void* address)
	{
		const auto game_address = reinterpret_cast<uint64_t>(address);
		constexpr auto inst_len = 3;

		const auto next_inst_addr = game_address + inst_len;
		const auto next_inst = *reinterpret_cast<uint32_t*>(next_inst_addr);

		if ((next_inst & 0xFF00FFFF) != 0xFF004583)
		{
			throw std::runtime_error(utils::string::va("Unable to patch intact basic block: %llX", game_address));
		}

		const auto other_frame_offset = static_cast<uint8_t>(next_inst >> 16);
		static const auto stub = utils::hook::assemble([](utils::hook::assembler& a)
			{
				a.push(rax);

				a.mov(rax, qword_ptr(rsp, 8));
				a.sub(rax, 2); // Skip the push we inserted

				a.push(rax);
				a.pushad64();

				a.mov(r8, qword_ptr(rsp, 0x88));
				a.mov(rcx, rax);
				a.mov(rdx, rbp);
				a.call_aligned(adjust_integrity_checksum);

				a.mov(qword_ptr(rsp, 0x80), rax);

				a.popad64();
				a.pop(rax);

				a.add(rsp, 8);

				a.mov(dword_ptr(rdx, rcx, 4), eax);

				a.pop(rax); // return addr
				a.xchg(rax, qword_ptr(rsp)); // switch with push

				a.add(dword_ptr(rbp, rax), 0xFFFFFFFF);

				a.mov(rax, dword_ptr(rdx, rcx, 4)); // restore rax

				a.ret();
			});

		// push other_frame_offset
		utils::hook::set<uint16_t>(game_address, static_cast<uint16_t>(0x6A | (other_frame_offset << 8)));
		utils::hook::call(game_address + 2, stub);
	}

	void patch_split_basic_block_integrity_check(void* address)
	{
		const auto game_address = reinterpret_cast<uint64_t>(address);
		constexpr auto inst_len = 3;

		const auto next_inst_addr = game_address + inst_len;

		if (*reinterpret_cast<uint8_t*>(next_inst_addr) != 0xE9)
		{
			throw std::runtime_error(utils::string::va("Unable to patch split basic block: %llX", game_address));
		}

		const auto jump_target = utils::hook::extract<void*>(reinterpret_cast<void*>(next_inst_addr + 1));
		const auto stub = utils::hook::assemble([jump_target](utils::hook::assembler& a)
			{
				a.push(rax);

				a.mov(rax, qword_ptr(rsp, 8));
				a.push(rax);

				a.pushad64();

				a.mov(r8, qword_ptr(rsp, 0x88));
				a.mov(rcx, rax);
				a.mov(rdx, rbp);
				a.call_aligned(adjust_integrity_checksum);

				a.mov(qword_ptr(rsp, 0x80), rax);

				a.popad64();
				a.pop(rax);

				a.add(rsp, 8);

				a.mov(dword_ptr(rdx, rcx, 4), eax);

				a.add(rsp, 8);

				a.jmp(jump_target);
			});

		utils::hook::call(game_address, stub);
	}

	void search_and_patch_integrity_checks()
	{
		// There seem to be 1219 results.
		// Searching them is quite slow.
		// Maybe precomputing that might be better?
		const auto intact_results = "89 04 8A 83 45 ? FF"_sig;
		const auto split_results = "89 04 8A E9"_sig;

		for (auto* i : intact_results)
		{
			patch_intact_basic_block_integrity_check(i);
		}

		for (auto* i : split_results)
		{
			patch_split_basic_block_integrity_check(i);
		}
	}
}