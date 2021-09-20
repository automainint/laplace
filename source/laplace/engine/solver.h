/*  laplace/engine/solver.h
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

#ifndef laplace_engine_solver_h
#define laplace_engine_solver_h

#include "basic_factory.h"
#include "basic_impact.h"
#include "world.h"

namespace laplace::engine {
  /*  World live execution solver.
   */
  class solver {
  public:
    static const bool default_allow_rewind;

    solver()  = default;
    ~solver() = default;

    void set_factory(ptr_factory f);
    void set_world(ptr_world w);

    void reset_factory();
    void reset_world();

    /*  Apply an event. May cause recalculation of the
     *  entire timeline if event time is set befure
     *  current time.
     *
     *  If event time not set, uses current time.
     *
     *  Returns adjusted event time.
     */
    auto apply(span_cbyte ev) -> uint64_t;

    void rewind_to(uint64_t time);

    void schedule(uint64_t delta);
    void join();

    /*  Allow to automatically rewind the timeline when an
     *  out-of-date impact added.
     */
    void allow_rewind(bool is_rewind_allowed);

    [[nodiscard]] auto is_rewind_allowed() const -> bool;

    [[nodiscard]] auto get_time() const -> uint64_t;
    [[nodiscard]] auto get_position() const -> sl::index;

    void               set_seed(seed_type seed);
    [[nodiscard]] auto get_seed() const -> seed_type;

    void clear_history();

    [[nodiscard]] auto get_history_count() const -> sl::whole;
    [[nodiscard]] auto get_history(sl::index n) const -> vbyte;

    [[nodiscard]] static auto generate_seed() -> seed_type;

  private:
    void adjust(uint64_t time);

    ptr_world         m_world;
    ptr_factory       m_factory;
    sl::vector<vbyte> m_history;

    uint64_t  m_time              = 0;
    sl::index m_position          = 0;
    bool      m_is_rewind_allowed = default_allow_rewind;
    seed_type m_seed              = 0;
  };

  using ptr_solver = std::shared_ptr<solver>;
}

#endif
