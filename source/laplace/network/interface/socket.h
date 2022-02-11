/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_network_interface_socket_h
#define laplace_network_interface_socket_h

#include "../defs.h"

namespace laplace::network {
  class socket_interface {
  public:
    class node {
    public:
      virtual ~node() noexcept = default;

      [[nodiscard]] virtual auto receive(span_byte,
                                         io_mode mode) noexcept
          -> sl::whole = 0;

      [[nodiscard]] virtual auto send(std::string_view address,
                                      uint16_t         port,
                                      span_cbyte) noexcept
          -> sl::whole = 0;

      [[nodiscard]] virtual auto get_port() const noexcept
          -> uint16_t = 0;

      [[nodiscard]] virtual auto get_remote_address() const noexcept
          -> std::string = 0;

      [[nodiscard]] virtual auto get_remote_port() const noexcept
          -> uint16_t = 0;

      [[nodiscard]] virtual auto is_msgsize() const noexcept
          -> bool = 0;

      [[nodiscard]] virtual auto is_connreset() const noexcept
          -> bool = 0;
    };

    virtual ~socket_interface() noexcept = default;

    [[nodiscard]] virtual auto open(uint16_t port) noexcept
        -> std::unique_ptr<node> = 0;
  };
}

#endif
