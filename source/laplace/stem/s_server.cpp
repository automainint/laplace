/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "server.h"

namespace laplace::stem {
  void server::set_verbose(bool is_verbose) noexcept { }

  void server::set_allowed_commands(span_cuint16 commands) noexcept {
  }

  auto server::is_connected() const noexcept -> bool {
    return false;
  }

  auto server::is_quit() const noexcept -> bool {
    return true;
  }

  auto server::get_port() const noexcept -> uint16_t {
    return network::any_port;
  }

  auto server::get_world() const noexcept -> engine::ptr_world {
    return {};
  }

  void server::listen(uint16_t port) noexcept { }

  void server::connect(std::string_view address,
                       uint16_t         port) noexcept { }

  void server::reconnect() noexcept { }

  void server::tick(sl::time time_elapsed) noexcept { }
}
