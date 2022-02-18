/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_NETWORK_UDP_IPV4_NODE_H
#define LAPLACE_NETWORK_UDP_IPV4_NODE_H

#include "../core/socket.h"
#include "interface/io.h"

namespace laplace::network {
  class udp_ipv4_node : public io_interface::node {
  public:
    log_handler log = get_global_log();
    
    udp_ipv4_node() noexcept;
    explicit udp_ipv4_node(uint16_t port) noexcept;

    ~udp_ipv4_node() noexcept override;

    void bind(uint16_t port = any_port) noexcept;

    [[nodiscard]] auto receive(span_byte data) noexcept
        -> sl::whole override;

    [[nodiscard]] auto send(std::string_view address, uint16_t port,
                            span_cbyte data) noexcept
        -> sl::whole override;

    [[nodiscard]] auto get_port() const noexcept -> uint16_t override;

    [[nodiscard]] auto get_remote_address() const noexcept
        -> std::string override;

    [[nodiscard]] auto get_remote_port() const noexcept
        -> uint16_t override;

    [[nodiscard]] auto is_size_error() const noexcept
        -> bool override;

    [[nodiscard]] auto is_connection_reset() const noexcept
        -> bool override;

  private:
    void init() noexcept;

    [[nodiscard]] auto send_internal(sockaddr_in const &name,
                                     span_cbyte         data) noexcept
        -> sl::whole;

    socket_t    m_socket;
    uint16_t    m_port = 0;
    sockaddr_in m_remote;
    bool        m_is_msgsize   = false;
    bool        m_is_connreset = false;
  };

  using ptr_udp_node = std::shared_ptr<udp_ipv4_node>;
}

#endif
