#pragma once

#include "../core/family.h"
#include <functional>

namespace laplace::format
{
    enum class mode
    {
        text,
        binary
    };

    using pack_type         = core::ptr_family;
    using const_pack_type   = core::cref_family;
    using fn_read           = std::function<vbyte (size_t)>;
    using fn_write          = std::function<size_t (cref_vbyte)>;
}
