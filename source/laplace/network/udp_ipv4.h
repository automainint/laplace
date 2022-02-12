/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_NETWORK_UDP_IPV4_H
#define LAPLACE_NETWORK_UDP_IPV4_H

#include "interface/io.h"

namespace laplace::network {
  class udp_ipv4 : public io_interface {
  public:
    ~udp_ipv4() noexcept override = default;

    [[nodiscard]] auto open(uint16_t port) noexcept
        -> std::unique_ptr<node> override;
  };
}

#endif
