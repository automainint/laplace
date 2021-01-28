#pragma once

#include "defs.h"
#include "../graphics/image.h"

/*  Laplace native image.
 */
namespace laplace::format::image
{
    using data_type = graphics::ptr_image;
    using const_data_type = graphics::cref_image;

    auto extract(const_pack_type pack) -> data_type;
    auto package(const_data_type data) -> pack_type;

    auto decode(fn_read read) -> data_type;
    auto encode(fn_write write, const_data_type data, mode encoding_mode) -> bool;
}
