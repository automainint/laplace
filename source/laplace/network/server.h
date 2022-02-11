/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_network_server_h
#define laplace_network_server_h

#include "clock.h"
#include "interface/execution.h"
#include "interface/log.h"
#include "interface/protocol.h"

namespace laplace::network {
  enum class server_state { prepare, action, pause };

  class server {
  public:
    static constexpr sl::index id_undefined    = -1;
    static constexpr sl::index index_undefined = -1;
    static constexpr sl::time  time_undefined  = -1;

    static const sl::time default_update_timeout_msec;
    static const sl::time default_ping_timeout_msec;
    static const sl::time default_connection_timeout_msec;

    server(server const &) = delete;
    server(server &&)      = delete;
    auto operator=(server const &) -> server & = delete;
    auto operator=(server &&) -> server & = delete;

    server() = default;
    virtual ~server();

    void setup_protocol_interface(
        protocol_interface const &in) noexcept;
    void setup_execution_interface(
        execution_interface const &in) noexcept;
    void setup_log_interface(log_interface const &in) noexcept;

    virtual void queue(span_cbyte seq);
    virtual void tick(sl::time delta_msec);

    virtual void reconnect();

    [[nodiscard]] auto get_state() const noexcept -> server_state;

    [[nodiscard]] auto get_bytes_sent() const noexcept -> sl::whole;
    [[nodiscard]] auto get_bytes_received() const noexcept
        -> sl::whole;
    [[nodiscard]] auto get_bytes_loss() const noexcept -> sl::whole;

    [[nodiscard]] auto is_connected() const noexcept -> bool;
    [[nodiscard]] auto is_quit() const noexcept -> bool;

    template <typename prime_impact_, typename... args_>
    inline void emit(args_... args) noexcept {
      this->queue(prime_impact_(args...).encode());
    }

  protected:
    void set_connected(bool is_connected) noexcept;
    void set_quit(bool is_quit) noexcept;

    void set_state(server_state state) noexcept;

    void reset_tick() noexcept;
    void add_bytes_sent(sl::whole count) noexcept;
    void add_bytes_received(sl::whole count) noexcept;
    void add_bytes_loss(sl::whole count) noexcept;

    [[nodiscard]] auto get_connection_timeout() const noexcept
        -> sl::time;

    [[nodiscard]] auto get_update_timeout() const noexcept
        -> sl::time;
    [[nodiscard]] auto get_ping_timeout() const noexcept -> sl::time;

    clock               m_clock;
    protocol_interface  m_proto = blank_protocol_interface();
    execution_interface m_exe   = blank_execution_interface();
    log_interface       m_log   = blank_log_interface();

  private:
    bool m_is_connected = false;
    bool m_is_quit      = false;

    sl::time m_connection_timeout_msec =
        default_connection_timeout_msec;
    sl::time m_update_timeout_msec = default_update_timeout_msec;
    sl::time m_ping_timeout_msec   = default_ping_timeout_msec;

    sl::whole m_bytes_sent     = 0;
    sl::whole m_bytes_received = 0;
    sl::whole m_bytes_loss     = 0;

    sl::whole m_total_sent     = 0;
    sl::whole m_total_received = 0;
    sl::whole m_total_loss     = 0;

    server_state m_state = server_state::prepare;
  };

  using ptr_server = std::shared_ptr<server>;
}

#endif
