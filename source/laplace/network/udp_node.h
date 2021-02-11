/*  laplace/network/udp_node.h
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

#ifndef __laplace__network_udp_node__
#define __laplace__network_udp_node__

#include "defs.h"

namespace laplace::network {
  class udp_node {
  public:
    udp_node();
    udp_node(uint16_t port);

    ~udp_node();

    void bind(uint16_t port = any_port);
    auto receive_to(uint8_t *p, size_t count, io_mode mode)
        -> size_t;
    auto receive(size_t count, io_mode mode) -> vbyte;
    auto send_to(std::string_view address, uint16_t port,
                 cref_vbyte seq) -> size_t;

    auto get_port() const -> uint16_t;
    auto get_remote_address() const -> std::string;
    auto get_remote_port() const -> uint16_t;

  private:
    void init();
    auto send_internal(const sockaddr_in &name, cref_vbyte seq)
        -> size_t;

    SOCKET      m_socket;
    uint16_t    m_port = 0;
    sockaddr_in m_remote;
  };

  using ptr_udp_node = std::shared_ptr<udp_node>;
}

#endif
