/*  laplace/network/remote.h
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

#ifndef laplace_network_remote_h
#define laplace_network_remote_h

#include "udp_server.h"

namespace laplace::network {
  /*  Remote server.
   */
  class remote final : public udp_server {
  public:
    remote();
    ~remote() final = default;

    void reconnect() override;

    void connect(std::string_view host_address, uint16_t host_port,
                 uint16_t client_port = any_port);

  private:
    auto perform_control(sl::index slot, span_cbyte seq)
        -> bool override;

    std::string m_host_address = localhost;
    uint16_t    m_host_port    = any_port;
    uint16_t    m_client_port  = any_port;
  };
}

#endif
