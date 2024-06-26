#pragma once
#include "eaglevm-core/virtual_machine/ir/x86/base_handler_gen.h"
#include "eaglevm-core/virtual_machine/ir/x86/base_x86_translator.h"

namespace eagle::ir::handler
{
    class cmp : public base_handler_gen
    {
    public:
        cmp();
        ir_insts gen_handler(handler_sig signature) override;
    };
}

namespace eagle::ir::lifter
{
    class cmp : public base_x86_translator
    {
        using base_x86_translator::base_x86_translator;

        void finalize_translate_to_virtual() override;
        translate_status encode_operand(codec::dec::op_imm op_imm, uint8_t idx) override;
    };
}