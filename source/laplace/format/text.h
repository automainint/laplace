#pragma once

#include "defs.h"

namespace laplace::format::text
{
    static constexpr auto s_commands    = u8"_commands";
    static constexpr auto s_function    = u8"_function";
    static constexpr auto s_arguments   = u8"_arguments";

    auto decode(fn_read read) -> pack_type;
    auto encode(fn_write write, const_pack_type data) -> bool;
}
