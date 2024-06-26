#include <utility>

#include "eaglevm-core/virtual_machine/machines/pidgeon/inst_regs.h"

#include "eaglevm-core/util/random.h"
#include "eaglevm-core/codec/zydis_helper.h"

#include "eaglevm-core/util/assert.h"

namespace eagle::virt::pidg
{
    inst_regs::inst_regs(const uint8_t temp_count, settings_ptr settings)
        : settings(std::move(settings))
    {
        stack_order = { };
        vm_order = { };

        number_of_vregs = 8;
        temp_variables = temp_count;
    }

    void inst_regs::init_reg_order()
    {
        // setup the scratch register stack order
        {
            // todo: find a safer way to do this instead of enumearting enum
            for (int i = codec::rax; i <= codec::r15; i++)
                stack_order[i - codec::rax] = static_cast<codec::reg>(i);

            // shuffle the scratch registers
            if (settings->get_randomize_stack_regs())
                std::ranges::shuffle(stack_order, util::ran_device::get().gen);
        }

        // completely separate | setup virtual machine registers
        {
            std::vector<codec::reg> vm_regs_order;
            vm_regs_order.append_range(stack_order);

            std::erase(vm_regs_order, codec::rax);
            std::erase(vm_regs_order, codec::rsp);

            if (settings->get_randomize_vm_regs())
                std::ranges::shuffle(vm_regs_order, util::ran_device::get().gen);

            // Add the undesirable registers back to the end of the array
            vm_regs_order.push_back(codec::rax);
            vm_regs_order.push_back(codec::rsp);

            std::ranges::copy(vm_regs_order, vm_order.begin());
        }
    }

    codec::reg inst_regs::get_reg(const uint8_t target) const
    {
        // this would be something like VIP, VSP, VTEMP, etc
        VM_ASSERT(target <= number_of_vregs - 1, "attempt to access invalid vreg");
        return vm_order[target];
    }

    codec::reg inst_regs::get_reg_temp(const uint8_t target) const
    {
        // this would be something like VTEMP, VTEMP2, VTEMP3
        VM_ASSERT(target <= temp_variables - 1, "attemp to access invalid vtemp");
        return vm_order[number_of_vregs + target];
    }

    std::vector<codec::reg> inst_regs::get_availiable_temp() const
    {
        std::vector<codec::reg> out;
        for(auto i = number_of_vregs; i < vm_order.size(); i++)
            out.push_back(vm_order[i]);

        return out;
    }

    std::pair<uint32_t, codec::reg_size> inst_regs::get_stack_displacement(const codec::reg reg) const
    {
        //determine 64bit version of register
        const codec::reg_size reg_size = get_reg_size(reg);
        const codec::reg bit64_reg = get_bit_version(reg, codec::reg_class::gpr_64);

        int found_index = 0;
        const auto reg_count = stack_order.size();

        for (int i = 0; i < reg_count; i++)
        {
            if (bit64_reg == stack_order[i])
            {
                found_index = i;
                break;
            }
        }

        int offset = 0;
        if (reg_size == codec::reg_size::bit_8)
        {
            if (is_upper_8(reg))
                offset = 1;
        }

        return { found_index * 8 + offset, reg_size };
    }

    void inst_regs::enumerate(const std::function<void(codec::reg)>& enumerable, const bool from_back)
    {
        if (from_back)
            std::ranges::for_each(stack_order, enumerable);
        else
            std::for_each(stack_order.rbegin(), stack_order.rend(), enumerable);
    }
}
