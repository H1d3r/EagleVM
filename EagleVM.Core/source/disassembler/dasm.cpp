#include "eaglevm-core/disassembler/dasm.h"

namespace eagle::dasm
{
    segment_dasm::segment_dasm(const uint64_t rva_base, uint8_t* buffer, const size_t size): rva_base(rva_base), instruction_buffer(buffer),
                                                                                             instruction_size(size)
    {
    }

    std::vector<basic_block_ptr> segment_dasm::explore_blocks(const uint64_t entry_rva)
    {
        std::vector<basic_block_ptr> collected_blocks;

        std::queue<uint64_t> explore_queue;
        explore_queue.push(entry_rva);

        while (!explore_queue.empty())
        {
            const size_t layer_size = explore_queue.size();
            for (int i = 0; i < layer_size; i++)
            {
                const uint32_t layer_rva = explore_queue.front();
                explore_queue.pop();

                set_current_rva(layer_rva);

                // check if we are in the middle of an already existing block
                // if so, we split up the block and we just continue
                for (auto& created_block : collected_blocks)
                {
                    if (layer_rva >= created_block->start_rva && layer_rva < created_block->end_rva_inc)
                    {
                        auto previous_block = std::make_shared<basic_block>();
                        previous_block->start_rva = created_block->start_rva;
                        previous_block->end_rva_inc = previous_block->start_rva;

                        created_block->start_rva = layer_rva;

                        while (previous_block->end_rva_inc < layer_rva)
                        {
                            auto block_inst = created_block->decoded_insts.front();
                            previous_block->decoded_insts.push_back(block_inst);
                            created_block->decoded_insts.erase(created_block->decoded_insts.begin());

                            previous_block->end_rva_inc += block_inst.instruction.length;
                        }

                        // this means there is some tricky control flow happening
                        // for instance, there may be an opaque branch to some garbage code
                        // another reason could be is we explored the wrong branch first of some obfuscated code and found garbage
                        // this will not happen with normally compiled code
                        VM_ASSERT(previous_block->end_rva_inc == layer_rva, "resulting jump is between an already explored instruction");
                        collected_blocks.push_back(previous_block);
                    }
                }

                auto block = std::make_shared<basic_block>();
                block->start_rva = get_current_rva();

                while (true)
                {
                    // we must do a check to see if our rva is at some already existing block,
                    // if so, we are going to end this block
                    const auto current_rva = get_current_rva();
                    for (const auto& created_block : collected_blocks)
                    {
                        if (current_rva >= created_block->start_rva && current_rva < created_block->end_rva_inc)
                        {
                            // we are inside
                            VM_ASSERT(current_rva == created_block->start_rva, "instruction overlap caused by seeking block");
                            break;
                        }
                    }

                    auto [decode_info, inst_size] = decode_current();
                    block->decoded_insts.push_back(decode_info);

                    set_current_rva(get_current_rva() + inst_size);

                    if (decode_info.instruction.meta.branch_type != ZYDIS_BRANCH_TYPE_NONE)
                    {
                        // branching instruction encountered
                        auto branches = get_branches();
                        for (auto& [is_resolved, is_ret, target_rva] : branches)
                            if (is_resolved)
                                explore_queue.push(target_rva);

                        break;
                    }
                }

                block->end_rva_inc = get_current_rva();
                collected_blocks.push_back(block);
            }
        }

        return collected_blocks;
    }

    basic_block_ptr segment_dasm::get_block(const uint32_t rva)
    {
        for (auto block : blocks)
        {
            if (rva == block->start_rva)
                return block;
        }

        return nullptr;
    }

    std::vector<basic_block_ptr>& segment_dasm::get_blocks()
    {
        return blocks;
    }

    std::pair<codec::dec::inst_info, uint8_t> segment_dasm::decode_current()
    {
        codec::dec::inst_info inst = codec::get_instruction(
            instruction_buffer,
            instruction_size,
            get_current_rva() - rva_base
        );

        return { inst, inst.instruction.length };
    }

    std::vector<branch_info_t> segment_dasm::get_branches()
    {
        auto [instruction, operands] = codec::get_instruction(
            instruction_buffer,
            instruction_size,
            get_current_rva() - rva_base
        );

        std::vector<branch_info_t> branches;
        if (instruction.mnemonic == codec::m_ret)
        {
            branches.push_back(branch_info_t{
                false,
                true,
                0
            });

            return branches;
        }

        if (is_jmp_or_jcc(static_cast<codec::mnemonic>(instruction.mnemonic)))
        {
            auto [target, op_idx] = codec::calc_relative_rva(instruction, operands, get_current_rva());
            branches.push_back(branch_info_t{
                op_idx != -1,
                false,
                target
            });

            if (instruction.mnemonic == codec::m_jmp)
                return branches;
        }

        branches.push_back(branch_info_t{
            true,
            false,
            get_current_rva() + instruction.length
        });

        return branches;
    }
}
