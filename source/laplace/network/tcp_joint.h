/*  laplace/network/tcp_joint.h
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

#ifndef laplace_network_tcp_tcp_joint_h
#define laplace_network_tcp_tcp_joint_h

#include "tcp_base.h"
#include <functional>

namespace laplace::network {
  class tcp_joint final : public tcp_base {
  public:
    using ref_tcp_joint = tcp_joint &;
    using event_connect = std::function<void(ref_tcp_joint j)>;

    tcp_joint(std::string_view address, uint16_t port);
    ~tcp_joint();

    void on_connect(event_connect fn);

    void tick();

  private:
    event_connect m_on_connect;
    char          m_address[address_size] = "";
    uint16_t      m_port                  = 0;
    bool          m_is_ready              = false;
  };

  using ref_tcp_joint = tcp_joint::ref_tcp_joint;
  using ptr_tcp_joint = std::shared_ptr<tcp_joint>;
}

#endif
