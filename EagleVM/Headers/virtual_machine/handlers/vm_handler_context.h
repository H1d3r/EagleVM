#pragma once
#include "virtual_machine/vm_register_manager.h"

class vm_handler_context
{
public:
    vm_register_manager* rm = nullptr;

    vm_handler_context(vm_register_manager* context);

    std::vector<zydis_encoder_request> generate_handler_return();
    void setup_keys();

private:
    // this will be implemented in the future to reduce the complexity of the project for the time being
    struct jmp_enc_constants
    {
        uint8_t values[3];
        uint8_t imms[3];
    } keys = {};
};