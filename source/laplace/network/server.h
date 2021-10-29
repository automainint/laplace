/*  laplace/network/server.h
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

#ifndef laplace_network_server_h
#define laplace_network_server_h

#include "../engine/basic_factory.h"
#include "../engine/solver.h"

namespace laplace::network {
  enum class server_state { prepare, action, pause };

  class server {
  public:
    static const bool      default_verbose;
    static const sl::time  default_tick_duration_msec;
    static const sl::time  default_update_timeout_msec;
    static const sl::time  default_ping_timeout_msec;
    static const sl::time  default_connection_timeout_msec;
    static const sl::whole default_overtake_factor;

    server(server const &) = delete;
    server(server &&)      = delete;
    auto operator=(server const &) -> server & = delete;
    auto operator=(server &&) -> server & = delete;

    server() = default;
    virtual ~server();

    void set_factory(engine::ptr_factory fac);
    void set_verbose(bool verbose) noexcept;

    virtual void queue(span_cbyte seq);
    virtual void tick(sl::time delta_msec);

    virtual void reconnect();

    [[nodiscard]] auto get_factory() const -> engine::ptr_factory;
    [[nodiscard]] auto get_solver() const -> engine::ptr_solver;
    [[nodiscard]] auto get_world() const -> engine::ptr_world;

    [[nodiscard]] auto get_ping() const noexcept -> sl::time;

    [[nodiscard]] auto get_state() const noexcept -> server_state;
    [[nodiscard]] auto get_tick_duration() const noexcept
        -> sl::whole;

    [[nodiscard]] auto get_bytes_sent() const noexcept -> sl::whole;
    [[nodiscard]] auto get_bytes_received() const noexcept
        -> sl::whole;
    [[nodiscard]] auto get_bytes_loss() const noexcept -> sl::whole;

    [[nodiscard]] auto is_connected() const noexcept -> bool;
    [[nodiscard]] auto is_quit() const noexcept -> bool;

    [[nodiscard]] auto is_verbose() const noexcept -> bool;

    template <typename prime_impact_, typename... args_>
    inline void emit(args_... args) {
      this->queue(engine::encode<prime_impact_>(args...));
    }

    template <typename factory_, typename... args_>
    inline void make_factory(args_... args) {
      this->set_factory(std::make_shared<factory_>(args...));
    }

  protected:
    void setup_solver();
    void setup_world();

    void set_connected(bool is_connected) noexcept;
    void set_quit(bool is_quit) noexcept;

    void set_tick_duration(sl::time tick_duration_msec) noexcept;
    void set_random_seed(engine::seed_type seed);
    void set_ping(sl::time ping_msec) noexcept;
    void set_state(server_state state) noexcept;

    void reset_tick() noexcept;
    void add_bytes_sent(sl::whole count) noexcept;
    void add_bytes_received(sl::whole count) noexcept;
    void add_bytes_loss(sl::whole count) noexcept;

    /*  Update tick timer. Returns time
     *  delta in ticks.
     */
    [[nodiscard]] auto adjust_delta(sl::time delta_msec) noexcept
        -> sl::time;

    [[nodiscard]] auto get_connection_timeout() const noexcept
        -> sl::time;

    [[nodiscard]] auto get_update_timeout() const noexcept
        -> sl::time;
    [[nodiscard]] auto get_ping_timeout() const noexcept -> sl::time;
    [[nodiscard]] auto get_overtake_factor() const noexcept
        -> sl::whole;

    void verb_queue(sl::index n, span_cbyte seq) const;
    void verb_slot(sl::index slot, sl::index n, span_cbyte seq) const;
    void dump(span_cbyte bytes) const;

  private:
    bool m_verbose      = default_verbose;
    bool m_is_connected = false;
    bool m_is_quit      = false;

    engine::ptr_factory m_factory;
    engine::ptr_solver  m_solver;
    engine::ptr_world   m_world;

    sl::time m_ping_msec          = 0;
    sl::time m_tick_clock_msec    = 0;
    sl::time m_tick_duration_msec = default_tick_duration_msec;
    sl::time m_connection_timeout_msec =
        default_connection_timeout_msec;
    sl::time  m_update_timeout_msec = default_update_timeout_msec;
    sl::time  m_ping_timeout_msec   = default_ping_timeout_msec;
    sl::whole m_overtake_factor     = default_overtake_factor;

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
