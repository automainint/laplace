/*  laplace/network/tcp_node.h
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

#ifndef laplace_network_tcp_node_h
#define laplace_network_tcp_node_h

#include "basic_tcp.h"
#include <functional>

namespace laplace::network {
  class tcp_node final : public basic_tcp {
  public:
    using ref_tcp_node = tcp_node &;
    using event_accept = std::function<void(ref_tcp_node n)>;

    tcp_node(uint16_t port = any_port);
    ~tcp_node();

    void on_accept(event_accept fn);

    /*  Listening loop.
     */
    void tick();

    /*  Close the current session and
     *  stop the listening loop.
     */
    void close_node();

    [[nodiscard]] auto is_connected() const -> bool;

    [[nodiscard]] auto get_port() const -> uint16_t;

    /*  Current session remote address.
     */
    [[nodiscard]] auto get_remote_address() const -> std::string;

    /*  Current session remote port.
     */
    [[nodiscard]] auto get_remote_port() const -> uint16_t;

  private:
    event_accept m_on_accept;
    uint16_t     m_port = 0;
    sockaddr_in  m_remote;
    SOCKET       m_listen;
  };

  using ref_tcp_node = tcp_node::ref_tcp_node;
  using ptr_tcp_node = std::shared_ptr<tcp_node>;
}

#endif
