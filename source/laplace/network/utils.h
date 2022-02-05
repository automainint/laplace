/*  Copyright (c) 2022 Mitya Selivanov
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

#include "../core/socket.h"
#include "defs.h"

namespace laplace::network {
  [[nodiscard]] auto clamp_chunk(sl::whole size) noexcept -> int;

  [[nodiscard]] auto to_string(::sockaddr const &a) noexcept
      -> std::string;

  [[nodiscard]] auto set_mode(socket_t &s, io_mode m) noexcept
      -> bool;
}

#endif
