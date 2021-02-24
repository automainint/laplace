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
    static constexpr bool     default_verbose                 = false;
    static constexpr uint64_t default_tick_duration_msec      = 10;
    static constexpr uint64_t default_connection_timeout_msec = 1000;
    static constexpr uint64_t default_update_timeout_msec     = 200;
    static constexpr uint64_t default_ping_timeout_msec       = 200;

    server()          = default;
    virtual ~server() = default;

    void set_factory(engine::ptr_factory fac);
    void set_verbose(bool verbose);

    virtual void queue(cref_vbyte seq);
    virtual void tick(size_t delta_msec);

    [[nodiscard]] auto get_factory() const -> engine::ptr_factory;
    [[nodiscard]] auto get_solver() const -> engine::ptr_solver;
    [[nodiscard]] auto get_world() const -> engine::ptr_world;

    [[nodiscard]] auto get_ping() const -> uint64_t;

    [[nodiscard]] auto get_state() const -> server_state;
    [[nodiscard]] auto get_tick_duration() -> size_t;

    [[nodiscard]] auto get_bytes_sent() const -> size_t;
    [[nodiscard]] auto get_bytes_received() const -> size_t;

    [[nodiscard]] auto is_verbose() const -> bool;
    [[nodiscard]] auto is_connected() const -> bool;

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

    void set_connected(bool is_connected);
    void set_tick_duration(uint64_t tick_duration_msec);
    void set_random_seed(engine::seed_type seed);
    void set_ping(uint64_t ping_msec);
    void set_state(server_state state);

    void reset_tick();
    void add_bytes_sent(size_t count);
    void add_bytes_received(size_t count);

    /*  Update tick timer. Returns time
     *  delta in ticks.
     */
    [[nodiscard]] auto adjust_delta(size_t delta_msec) -> uint64_t;

    [[nodiscard]] auto get_connection_timeout() const -> uint64_t;
    [[nodiscard]] auto get_update_timeout() const -> uint64_t;
    [[nodiscard]] auto get_ping_timeout() const -> uint64_t;

    void dump(cref_vbyte bytes);

  private:
    bool m_verbose      = default_verbose;
    bool m_is_connected = false;

    engine::ptr_factory m_factory;
    engine::ptr_solver  m_solver;
    engine::ptr_world   m_world;

    uint64_t m_ping_msec               = 0;
    uint64_t m_tick_clock_msec         = 0;
    uint64_t m_tick_duration_msec      = default_tick_duration_msec;
    uint64_t m_connection_timeout_msec = default_connection_timeout_msec;
    uint64_t m_update_timeout_msec     = default_update_timeout_msec;
    uint64_t m_ping_timeout_msec       = default_ping_timeout_msec;

    size_t m_bytes_sent     = 0;
    size_t m_bytes_received = 0;

    server_state m_state = server_state::prepare;
  };

  using ptr_server = std::shared_ptr<server>;
}

#endif
