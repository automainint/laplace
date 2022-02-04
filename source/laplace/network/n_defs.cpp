/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "defs.h"

namespace laplace::network {
  sl::whole const max_chunk_size = 0x1000;
  char const      localhost[]    = "127.0.0.1";
  uint16_t const  any_port       = 0;
}
