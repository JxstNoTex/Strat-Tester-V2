#include <std_include.hpp>
#include "game/game.hpp"
#include "loader/component_loader.hpp"
#include "utils/string.hpp"
#include "arxan.hpp"

namespace Arxan
{
	namespace
	{
		const std::vector<std::pair<uint8_t*, size_t>>& get_text_sections()
		{
			static const std::vector<std::pair<uint8_t*, size_t>> text = []
				{
					std::vector<std::pair<uint8_t*, size_t>> texts{};

					const utils::nt::library game{};
					for (const auto& section : game.get_section_headers())
					{
						if (section->Characteristics & IMAGE_SCN_MEM_EXECUTE)
						{
							texts.emplace_back(game.get_ptr() + section->VirtualAddress, section->Misc.VirtualSize);
						}
					}

					return texts;
				}();

				return text;
		}

		bool is_in_texts(const uint64_t addr)
		{
			const auto& texts = get_text_sections();
			for (const auto& text : texts)
			{
				const auto start = reinterpret_cast<ULONG_PTR>(text.first);
				if (addr >= start && addr <= (start + text.second))
				{
					return true;
				}
			}

			return false;
		}

		bool is_in_texts(const void* addr)
		{
			return is_in_texts(reinterpret_cast<uint64_t>(addr));
		}

		struct integrity_handler_context
		{
			uint32_t* computed_checksum;
			uint32_t* original_checksum;
		};

		bool is_on_stack(uint8_t* stack_frame, const void* pointer)
		{
			const auto stack_value = reinterpret_cast<uint64_t>(stack_frame);
			const auto pointer_value = reinterpret_cast<uint64_t>(pointer);

			const auto diff = static_cast<int64_t>(stack_value - pointer_value);
			return std::abs(diff) < 0x1000;
		}

		// Pretty trashy, but working, heuristic to search the integrity handler context
		bool is_handler_context(uint8_t* stack_frame, const uint32_t computed_checksum, const uint32_t frame_offset)
		{
			const auto* potential_context = reinterpret_cast<integrity_handler_context*>(stack_frame + frame_offset);
			return is_on_stack(stack_frame, potential_context->computed_checksum)
				&& *potential_context->computed_checksum == computed_checksum
				&& is_in_texts(potential_context->original_checksum);
		}

		integrity_handler_context* search_handler_context(uint8_t* stack_frame, const uint32_t computed_checksum)
		{
			for (uint32_t frame_offset = 0; frame_offset < 0x90; frame_offset += 8)
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
			const auto handler_address = game::derelocate(return_address - 5);
			const auto* context = search_handler_context(stack_frame, current_checksum);

			if (!context)
			{
				game::show_error(utils::string::va("No frame offset for: %llX", handler_address));
				TerminateProcess(GetCurrentProcess(), 0xBAD);
				return current_checksum;
			}

			const auto correct_checksum = *context->original_checksum;
			*context->computed_checksum = correct_checksum;

			if (current_checksum != correct_checksum)
			{
#ifndef NDEBUG
				/*printf("Adjusting checksum (%llX): %X -> %X\n", handler_address,
					   current_checksum, correct_checksum);*/
#endif
			}

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
			for (const auto i : intact_integrity_check_blocks)
			{
				patch_intact_basic_block_integrity_check(reinterpret_cast<void*>(game::relocate(i)));
			}

			for (const auto i : split_integrity_check_blocks)
			{
				patch_split_basic_block_integrity_check(reinterpret_cast<void*>(game::relocate(i)));
			}
		}
	}

	class component final : public component_interface
	{
	public:
		void post_start() override
		{
			search_and_patch_integrity_checks();
		}

	};
}

REGISTER_COMPONENT(Arxan::component)