/*  laplace/format/model.h
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_format_model_h
#define laplace_format_model_h

#include "../render/model.h"
#include "defs.h"

/*  Laplace native 3D model format.
 */
namespace laplace::format::model {
  using data_type       = render::ptr_model;
  using const_data_type = render::cref_model;

  auto extract(const_pack_type pack) -> data_type;
  auto package(const_data_type data) -> pack_type;

  auto decode(fn_read read) -> data_type;
  auto encode(fn_write write, const_data_type data, mode encoding_mode)
      -> bool;
}

#endif
