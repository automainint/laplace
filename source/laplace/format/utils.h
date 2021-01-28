#pragma once

#include "defs.h"

namespace laplace::format
{
    auto wrap(ref_istream in) -> fn_read;
    auto wrap(ref_ostream out) -> fn_write;
}

#include "utils.impl.h"
