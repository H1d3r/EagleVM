#include "virtual_machine/handlers/vm_handler_entry.h"

class ia32_dec_handler : public vm_handler_entry
{
public:
    ia32_dec_handler();

private:
    virtual dynamic_instructions_vec construct_single(reg_size size) override;
};