#include "eaglevm-core/virtual_machine/machines/base_machine.h"
#include "eaglevm-core/virtual_machine/ir/block.h"

namespace eagle::virt
{
    asmb::code_container_ptr base_machine::lift_block(const ir::block_il_ptr& block, bool scatter)
    {
        const size_t command_count = block->get_command_count();

        const asmb::code_container_ptr code = asmb::code_container::create("block_begin " + command_count, true);
        for (size_t i = 0; i < command_count; i++)
        {
            const ir::base_command_ptr command = block->get_command(i);
            switch (command->get_command_type())
            {
                case ir::command_type::vm_enter:
                    handle_cmd(code, std::static_pointer_cast<ir::cmd_vm_enter>(command));
                    break;
                case ir::command_type::vm_exit:
                    handle_cmd(code, std::static_pointer_cast<ir::cmd_vm_exit>(command));
                    break;
                case ir::command_type::vm_handler_call:
                    handle_cmd(code, std::static_pointer_cast<ir::cmd_handler_call>(command));
                    break;
                case ir::command_type::vm_reg_load:
                    handle_cmd(code, std::static_pointer_cast<ir::cmd_context_load>(command));
                    break;
                case ir::command_type::vm_reg_store:
                    handle_cmd(code, std::static_pointer_cast<ir::cmd_context_store>(command));
                    break;
                case ir::command_type::vm_push:
                    handle_cmd(code, std::static_pointer_cast<ir::cmd_push>(command));
                    break;
                case ir::command_type::vm_pop:
                    handle_cmd(code, std::static_pointer_cast<ir::cmd_pop>(command));
                    break;
                case ir::command_type::vm_mem_read:
                    handle_cmd(code, std::static_pointer_cast<ir::cmd_mem_read>(command));
                    break;
                case ir::command_type::vm_mem_write:
                    handle_cmd(code, std::static_pointer_cast<ir::cmd_mem_write>(command));
                    break;
                case ir::command_type::vm_context_load:
                    handle_cmd(code, std::static_pointer_cast<ir::cmd_context_load>(command));
                    break;
                case ir::command_type::vm_exec_x86:
                    handle_cmd(code, std::static_pointer_cast<ir::cmd_x86_exec>(command));
                    break;
                case ir::command_type::vm_exec_dynamic_x86:
                    handle_cmd(code, std::static_pointer_cast<ir::cmd_x86_dynamic>(command));
                    break;
                case ir::command_type::vm_rflags_load:
                    handle_cmd(code, std::static_pointer_cast<ir::cmd_rflags_load>(command));
                    break;
                case ir::command_type::vm_rflags_store:
                    handle_cmd(code, std::static_pointer_cast<ir::cmd_rflags_store>(command));
                    break;
                case ir::command_type::vm_sx:
                    handle_cmd(code, std::static_pointer_cast<ir::cmd_sx>(command));
                    break;
                case ir::command_type::vm_branch:
                    handle_cmd(code, std::static_pointer_cast<ir::cmd_branch>(command));
                    break;
            }
        }

        return code;
    }

    ir::ir_size base_machine::to_ir_size(const codec::reg_size reg_size)
    {
        switch (reg_size)
        {
            case codec::empty:
                return ir::ir_size::none;
            case codec::bit_512:
                return ir::ir_size::bit_512;
            case codec::bit_256:
                return ir::ir_size::bit_256;
            case codec::bit_128:
                return ir::ir_size::bit_128;
            case codec::bit_64:
                return ir::ir_size::bit_64;
            case codec::bit_32:
                return ir::ir_size::bit_32;
            case codec::bit_16:
                return ir::ir_size::bit_16;
            case codec::bit_8:
                return ir::ir_size::bit_8;
        }
    }

    codec::reg_size base_machine::to_reg_size(const ir::ir_size ir_size)
    {
        switch (ir_size)
        {
            case ir::ir_size::bit_512:
                return codec::reg_size::bit_512;
            case ir::ir_size::bit_256:
                return codec::reg_size::bit_256;
            case ir::ir_size::bit_128:
                return codec::reg_size::bit_128;
            case ir::ir_size::bit_64:
                return codec::reg_size::bit_64;
            case ir::ir_size::bit_32:
                return codec::reg_size::bit_32;
            case ir::ir_size::bit_16:
                return codec::reg_size::bit_16;
            case ir::ir_size::bit_8:
                return codec::reg_size::bit_8;
            case ir::ir_size::none:
                return codec::reg_size::empty;
        }
    }

    codec::mnemonic base_machine::to_jump_mnemonic(const ir::exit_condition condition)
    {
        switch (condition)
        {
            case ir::exit_condition::jb:
                return codec::m_jb;
            case ir::exit_condition::jbe:
                return codec::m_jbe;
            case ir::exit_condition::jcxz:
                return codec::m_jcxz;
            case ir::exit_condition::jecxz:
                return codec::m_jecxz;
            case ir::exit_condition::jknzd:
                return codec::m_jknzd;
            case ir::exit_condition::jkzd:
                return codec::m_jkzd;
            case ir::exit_condition::jl:
                return codec::m_jl;
            case ir::exit_condition::jle:
                return codec::m_jle;
            case ir::exit_condition::jmp:
                return codec::m_jmp;
            case ir::exit_condition::jnb:
                return codec::m_jnb;
            case ir::exit_condition::jnbe:
                return codec::m_jnbe;
            case ir::exit_condition::jnl:
                return codec::m_jnl;
            case ir::exit_condition::jnle:
                return codec::m_jnle;
            case ir::exit_condition::jno:
                return codec::m_jno;
            case ir::exit_condition::jnp:
                return codec::m_jnp;
            case ir::exit_condition::jns:
                return codec::m_jns;
            case ir::exit_condition::jnz:
                return codec::m_jnz;
            case ir::exit_condition::jo:
                return codec::m_jo;
            case ir::exit_condition::jp:
                return codec::m_jp;
            case ir::exit_condition::jrcxz:
                return codec::m_jrcxz;
            case ir::exit_condition::js:
                return codec::m_js;
            case ir::exit_condition::jz:
                return codec::m_jz;
            default:
            {
                assert("invalid exit condition reached");
                return codec::m_invalid;
            }
        }
    }
}
