/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

/*  Trivial class that contains event order information.
 *  Allows sorting tree-structured events.
 */

#ifndef laplace_engine_eventorder_h
#define laplace_engine_eventorder_h

#include "../core/defs.h"

namespace laplace::engine {
  class eventorder {
  public:
    /*  Maximum event tree depth.
     */
    static constexpr uint8_t max_depth = 16;

    static constexpr sl::index tick = 0;
    static constexpr sl::index root = 1;

    constexpr eventorder() = default;
    constexpr eventorder(sl::index n);
    ~eventorder() = default;

    constexpr eventorder(const eventorder &) = default;
    constexpr eventorder(eventorder &&)      = default;
    constexpr auto operator                  =(const eventorder &)
        -> eventorder                      & = default;
    constexpr auto operator=(eventorder &&) -> eventorder & = default;

    /*  Spawn a child event.
     */
    [[nodiscard]] constexpr auto spawn(sl::whole &child_count) const
        -> eventorder;

    [[nodiscard]] constexpr auto operator<(
        eventorder const &order) const -> bool;

    [[nodiscard]] constexpr auto get_index() const -> sl::index;

  private:
    constexpr eventorder(eventorder const &parent,
                         sl::index         child_index);

    std::array<sl::index, max_depth> m_indices = { 0 };
    uint8_t                          m_size    = 0;
  };

  using cref_eventorder = const eventorder &;
}

#include "eventorder.impl.h"

#endif
