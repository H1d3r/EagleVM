#include "eaglevm-core/virtual_machine/ir/commands/cmd_mem_read.h"

namespace eagle::ir
{
    cmd_mem_read::cmd_mem_read(const ir_size size)
        : base_command(command_type::vm_mem_read), size(size)
    {
    }

    ir_size cmd_mem_read::get_read_size() const
    {
        return size;
    }
}
