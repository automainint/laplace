/*  laplace/format/image.h
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

#ifndef laplace_format_image_h
#define laplace_format_image_h

#include "../graphics/image.h"
#include "defs.h"

/*  Laplace native image.
 */
namespace laplace::format::image {
  using data_type       = graphics::ptr_image;
  using const_data_type = graphics::cref_image;

  auto extract(const_pack_type pack) -> data_type;
  auto package(const_data_type data) -> pack_type;

  auto decode(fn_read read) -> data_type;
  auto encode(fn_write write, const_data_type data, mode encoding_mode)
      -> bool;
}

#endif
