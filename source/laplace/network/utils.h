/*  laplace/network/utils.h
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

#ifndef laplace_network_utils_h
#define laplace_network_utils_h

#include "defs.h"

namespace laplace::network {
  auto clamp_chunk(size_t size) -> int;
  auto to_string(const sockaddr &a) -> std::string;
  auto set_mode(SOCKET &s, io_mode m) -> bool;
}

#endif
