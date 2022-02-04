/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "udp_ipv4_interface.h"

#include "udp_ipv4_node.h"

namespace laplace::network {
  using std::unique_ptr, std::make_unique;

  auto udp_ipv4_interface::open(uint16_t port) noexcept
      -> unique_ptr<node> {
    return make_unique<udp_ipv4_node>(port);
  }
}
