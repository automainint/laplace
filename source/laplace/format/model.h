#pragma once

#include "defs.h"
#include "../render/model.h"

/*  Laplace native 3D model format.
 */
namespace laplace::format::model
{
    using data_type = render::ptr_model;
    using const_data_type = render::cref_model;

    auto extract(const_pack_type pack) -> data_type;
    auto package(const_data_type data) -> pack_type;

    auto decode(fn_read read) -> data_type;
    auto encode(fn_write write, const_data_type data, mode encoding_mode) -> bool;
}
