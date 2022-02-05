/*  Copyright (c) 2022 Mitya Selivanov
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

namespace laplace::engine {
  /*  World live execution solver.
   */
  class solver {
  public:
    using fn_reset    = std::function<void()>;
    using fn_seed     = std::function<void(seed_type)>;
    using fn_tick     = std::function<void(sl::time)>;
    using fn_schedule = std::function<void(sl::time)>;
    using fn_join     = std::function<void()>;
    using fn_queue    = std::function<void(ptr_impact)>;
    using fn_decode   = std::function<ptr_impact(span_cbyte)>;

    solver() noexcept  = default;
    ~solver() noexcept = default;

    void on_reset(fn_reset callback) noexcept;
    void on_seed(fn_seed callback) noexcept;
    void on_tick(fn_tick callback) noexcept;
    void on_schedule(fn_schedule callback) noexcept;
    void on_join(fn_join callback) noexcept;
    void on_queue(fn_queue callback) noexcept;
    void on_decode(fn_decode callback) noexcept;

    auto apply(span_cbyte event) noexcept -> sl::time;
    void rewind_to(sl::time time) noexcept;
    void schedule(sl::time delta) noexcept;

    [[nodiscard]] auto get_time() const noexcept -> sl::time;
    [[nodiscard]] auto get_position() const noexcept -> sl::index;

    void               set_seed(seed_type seed) noexcept;
    [[nodiscard]] auto get_seed() const noexcept -> seed_type;

    void clear_history() noexcept;

    [[nodiscard]] auto get_history() const noexcept
        -> std::span<vbyte const>;

    [[nodiscard]] static auto generate_seed() noexcept -> seed_type;

  private:
    sl::vector<vbyte> m_history;

    sl::time  m_time     = 0;
    sl::index m_position = 0;
    seed_type m_seed     = 0;

    fn_reset    m_reset;
    fn_seed     m_set_seed;
    fn_tick     m_tick;
    fn_schedule m_schedule;
    fn_join     m_join;
    fn_queue    m_queue;
    fn_decode   m_decode;
  };

  using ptr_solver = std::shared_ptr<solver>;
}

#endif
