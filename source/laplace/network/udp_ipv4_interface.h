/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_network_udp_ipv4_interface_h
#define laplace_network_udp_ipv4_interface_h

#include "socket_interface.h"

namespace laplace::network {
  class udp_ipv4_interface : public socket_interface {
  public:
    ~udp_ipv4_interface() noexcept override = default;

    [[nodiscard]] auto open(uint16_t port) noexcept
        -> std::unique_ptr<node> override;
  };
}

#endif
