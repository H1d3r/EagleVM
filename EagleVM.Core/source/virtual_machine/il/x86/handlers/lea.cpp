#include "eaglevm-core/virtual_machine/il/x86/handlers/lea.h"

namespace eagle::il::handler
{
    lea::lea()
    {
        entries = {
            { codec::gpr_64, 1 },
            { codec::gpr_32, 1 },
            { codec::gpr_16, 1 },
        };
    }

    il_insts lea::gen_il(codec::reg_class size, uint8_t operands)
    {
    }
}
