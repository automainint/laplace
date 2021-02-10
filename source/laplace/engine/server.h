/*  laplace/engine/server.h
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

#ifndef __laplace__engine_server__
#define __laplace__engine_server__

#include "basic_factory.h"
#include "basic_impact.h"
#include "solver.h"

namespace laplace::engine {
  enum class server_state { prepare, action, pause };

  class server {
  public:
    static constexpr bool     default_verbose = false;
    static constexpr uint64_t default_tick_duration_msec = 10;

    server()          = default;
    virtual ~server() = default;

    void set_factory(ptr_factory fac);
    void set_verbose(bool verbose);

    virtual void queue(cref_vbyte seq);
    virtual void tick(size_t delta_msec);

    auto get_factory() const -> ptr_factory;
    auto get_solver() const -> ptr_solver;
    auto get_world() const -> ptr_world;

    auto get_ping() const -> uint64_t;

    auto get_state() const -> server_state;
    auto get_tick_duration() -> size_t;

    auto is_verbose() const -> bool;
    auto is_connected() const -> bool;

  protected:
    void setup_solver();
    void setup_world();

    void set_connected(bool is_connected);
    void set_tick_duration(uint64_t tick_duration_msec);
    void set_random_seed(seed_type seed);
    void set_ping(uint64_t ping_msec);
    void set_state(server_state state);

    /*  Update tick timer. Returns time
     *  delta in ticks.
     */
    auto adjust_delta(size_t delta_msec) -> uint64_t;

    void dump(cref_vbyte bytes);

  private:
    bool m_verbose      = default_verbose;
    bool m_is_connected = false;

    ptr_factory m_factory;
    ptr_solver  m_solver;
    ptr_world   m_world;

    uint64_t m_ping_msec          = 0;
    uint64_t m_tick_clock_msec    = 0;
    uint64_t m_tick_duration_msec = default_tick_duration_msec;
    server_state m_state          = server_state::prepare;
  };

  using ptr_server = std::shared_ptr<server>;
}

#endif
