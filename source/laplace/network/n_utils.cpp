/*  Copyright (c) 2022 Mitya Selivanov
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
  using std::min, std::max, std::string;

  auto clamp_chunk(sl::whole size) noexcept -> int {
    return static_cast<int>(
        max<sl::whole>(0, min(size, max_chunk_size)));
  }

  auto to_string(::sockaddr const &a) noexcept -> string {
    char buf[64] = {};

    if (a.sa_family == AF_INET) {
      auto const &ip4 = reinterpret_cast<::sockaddr_in const &>(a);
      ::inet_ntop(AF_INET, &ip4.sin_addr, buf, sizeof buf);
    } else if (a.sa_family == AF_INET6) {
      auto const &ip6 = reinterpret_cast<::sockaddr_in6 const &>(a);
      ::inet_ntop(AF_INET6, &ip6.sin6_addr, buf, sizeof buf);
    } else {
      error_("Unknown address family.", __FUNCTION__);
      buf[0] = '\0';
    }

    return buf;
  }
}
