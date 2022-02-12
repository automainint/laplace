/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_NETWORK_INTERFACE_IO_H
#define LAPLACE_NETWORK_INTERFACE_IO_H

#include "../defs.h"

namespace laplace::network {
  class io_interface {
  public:
    class node {
    public:
      virtual ~node() noexcept = default;

      [[nodiscard]] virtual auto receive(span_byte) noexcept
          -> sl::whole = 0;

      [[nodiscard]] virtual auto send(std::string_view, uint16_t,
                                      span_cbyte) noexcept
          -> sl::whole = 0;

      [[nodiscard]] virtual auto get_port() const noexcept
          -> uint16_t = 0;

      [[nodiscard]] virtual auto get_remote_address() const noexcept
          -> std::string = 0;

      [[nodiscard]] virtual auto get_remote_port() const noexcept
          -> uint16_t = 0;

      [[nodiscard]] virtual auto is_size_error() const noexcept
          -> bool = 0;

      [[nodiscard]] virtual auto is_connection_reset() const noexcept
          -> bool = 0;
    };

    virtual ~io_interface() noexcept = default;

    [[nodiscard]] virtual auto open(uint16_t) noexcept
        -> std::unique_ptr<node> = 0;
  };

  using ptr_io   = std::shared_ptr<io_interface>;
  using ptr_node = std::unique_ptr<io_interface::node>;
}

#endif
