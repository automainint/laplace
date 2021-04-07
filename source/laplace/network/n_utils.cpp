/*  laplace/network/n_utils.cpp
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

#include "utils.h"
#include <algorithm>

namespace laplace::network {
  using std::min, std::string;

  auto clamp_chunk(size_t size) noexcept -> int {
    return static_cast<int>(min(size, max_chunk_size));
  }

  auto to_string(const sockaddr &a) noexcept -> string {
    char buf[64] = {};

    if (a.sa_family == AF_INET) {
      const auto &ip4 = reinterpret_cast<const sockaddr_in &>(a);
      ::inet_ntop(AF_INET, &ip4.sin_addr, buf, sizeof buf);
    } else {
      buf[0] = '\0';
    }

    return buf;
  }

  auto set_mode(socket_t &s, network::io_mode m) noexcept -> bool {
    if (m == sync) {
      if (socket_set_blocking(s) != 0) {
        ::closesocket(s);
        s = -1;

        return false;
      }
    } else if (m == async) {
      if (socket_set_nonblocking(s) != 0) {
        ::closesocket(s);
        s = -1;

        return false;
      }
    } else {
      error_("Invalid IO mode.", __FUNCTION__);
      return false;
    }

    return true;
  }
}
