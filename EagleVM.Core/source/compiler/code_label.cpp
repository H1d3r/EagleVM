#include "eaglevm-core/compiler/code_label.h"

namespace eagle::asmb
{
    code_label_ptr code_label::create()
    {
        return std::make_shared<code_label>();
    }

    code_label_ptr code_label::create(const std::string& label_name, bool generate_comments)
    {
        return std::make_shared<code_label>(label_name, generate_comments);
    }

    std::string code_label::get_name()
    {
        return name;
    }

    void code_label::set_name(const std::string& new_name)
    {
        name = new_name;
    }

    bool code_label::get_is_named() const
    {
        return is_named;
    }

    int64_t code_label::get_relative_address() const
    {
        return relative_address;
    }

    int64_t code_label::get_address() const
    {
        return relative_address + base_address;
    }

    void code_label::set_address(uint64_t base, uint64_t address)
    {
        base_address = base;
        relative_address = address;
    }

    code_label::code_label()
    {
        is_named = false;
        name = "";

        relative_address = 0;
        base_address = 0;
    }

    code_label::code_label(const std::string& label_name, const bool generate_comments)
    {
        is_named = generate_comments;
        name = label_name;

        relative_address = 0;
        base_address = 0;
    }
}