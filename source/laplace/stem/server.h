/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_stem_server_h
#define laplace_stem_server_h

#include "../engine/world.h"
#include "../network/server.h"

namespace laplace::stem {
  class server {
  public:
    void set_verbose(bool is_verbose) noexcept;
    void set_allowed_commands(span_cuint16 commands) noexcept;

    [[nodiscard]] auto is_connected() const noexcept -> bool;
    [[nodiscard]] auto is_quit() const noexcept -> bool;
    [[nodiscard]] auto get_port() const noexcept -> uint16_t;
    [[nodiscard]] auto get_world() const noexcept
        -> engine::ptr_world;

    void listen(uint16_t port) noexcept;
    void connect(std::string_view address, uint16_t port) noexcept;
    void reconnect() noexcept;
    void tick(sl::time time_elapsed) noexcept;

    template <typename factory>
    void make_factory() noexcept;

    template <typename impact, typename... args_>
    void emit(args_... args) noexcept;
  };

  using host   = server;
  using remote = server;

  using ptr_server = std::shared_ptr<server>;
}

#include "server.impl.h"

#endif
