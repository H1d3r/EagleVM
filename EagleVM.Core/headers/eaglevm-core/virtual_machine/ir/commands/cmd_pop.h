#pragma once
#include <utility>

#include "eaglevm-core/virtual_machine/ir/commands/base_command.h"

namespace eagle::ir
{
    class cmd_pop : public base_command
    {
    public:
        explicit cmd_pop(discrete_store_ptr reg_dest, ir_size size);

        discrete_store_ptr get_destination_reg();
        ir_size get_size() const;

    private:
        discrete_store_ptr dest_reg;
        ir_size size;
    };
}
