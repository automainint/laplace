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

#ifndef laplace_network_udp_node_h
#define laplace_network_udp_node_h

#include "defs.h"

namespace laplace::network {
  class udp_node {
  public:
    udp_node();
    udp_node(uint16_t port);

    ~udp_node();

    void bind(uint16_t port = any_port);

    [[nodiscard]] auto receive_to( //
        uint8_t *p,                //
        size_t   count,            //
        io_mode  mode) -> size_t;

    [[nodiscard]] auto receive( //
        size_t  count,          //
        io_mode mode) -> vbyte;

    [[nodiscard]] auto send_to(   //
        std::string_view address, //
        uint16_t         port,    //
        cref_vbyte       seq) -> size_t;

    [[nodiscard]] auto get_port() const -> uint16_t;
    [[nodiscard]] auto get_remote_address() const -> std::string;
    [[nodiscard]] auto get_remote_port() const -> uint16_t;

    [[nodiscard]] auto is_msgsize() const noexcept -> bool;
    [[nodiscard]] auto is_connreset() const noexcept -> bool;

  private:
    void init();

    [[nodiscard]] auto send_internal( //
        const sockaddr_in &name,      //
        cref_vbyte         seq) -> size_t;

    SOCKET      m_socket;
    uint16_t    m_port = 0;
    sockaddr_in m_remote;
    bool        m_is_msgsize   = false;
    bool        m_is_connreset = false;
  };

  using ptr_udp_node = std::shared_ptr<udp_node>;
}

#endif
