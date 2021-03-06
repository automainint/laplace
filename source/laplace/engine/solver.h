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

#include "basic_impact.h"
#include "world.h"

namespace laplace::engine {
  /*  World live execution solver.
   */
  class solver {
  public:
    static constexpr bool default_is_rewind_allowed = false;

    solver()  = default;
    ~solver() = default;

    void set_world(ptr_world w);
    void reset_world();

    /*  Add Impact to queue. Adjust the Impact
     *  time due performing if needed.
     *
     *  If an Impact time is set, it can change
     *  the previous World state, which will
     *  cause rewind and recalculating of the
     *  entire timeline.
     */
    void apply(ptr_impact imp);

    void rewind_to(uint64_t time);

    /*  Equals to schedule and join.
     */
    void solve(uint64_t delta);

    void schedule(uint64_t delta);
    void join();

    /*  Allow to automatically rewind the timeline when an
     *  out-of-date impact added.
     */
    void allow_rewind(bool is_rewind_allowed);

    [[nodiscard]] auto is_rewind_allowed() const -> bool;

    [[nodiscard]] auto get_time() const -> uint64_t;
    [[nodiscard]] auto get_position() const -> size_t;

    void               set_seed(seed_type seed);
    [[nodiscard]] auto get_seed() const -> seed_type;

    void clear_history();

    [[nodiscard]] auto get_history_count() const -> size_t;
    [[nodiscard]] auto get_history(size_t index) const -> ptr_impact;

    [[nodiscard]] static auto generate_seed() -> seed_type;

  private:
    void adjust(uint64_t time);

    ptr_world   m_world;
    vptr_impact m_history;

    uint64_t  m_time              = 0;
    size_t    m_position          = 0;
    bool      m_is_rewind_allowed = default_is_rewind_allowed;
    seed_type m_seed              = 0;
  };

  using ptr_solver = std::shared_ptr<solver>;
}

#endif
