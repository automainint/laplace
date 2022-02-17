/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_ENGINE_BASIC_IMPACT_H
#define LAPLACE_ENGINE_BASIC_IMPACT_H

#include "../core/utils.h"
#include "access/world.h"
#include "eventorder.h"

namespace laplace::engine {
  /*  World event compute atom. Impacts can be SEQUENTIALLY
   *  INCONSISTENT, that allows to use parallel computation.
   *
   *  For complete determinism all the Impacts should have the
   *  order index sequence, even the async ones, if they may
   *  potentially create some sync Impacts.
   *
   *  When create a new Impact:
   *
   *  ```
   *  void my_impact::perform(access::world w) const {
   *      sl::whole child_count = 0;
   *      // ...
   *
   *      //  Create an Impact.
   *      auto ev = make_shared<my_new_impact>();
   *
   *      //  Calculate the order index sequence.
   *      auto order = order_of_child(child_count);
   *
   *      //  Set the order.
   *      ev->set_order(order);
   *
   *      //  Queue the new Impact.
   *      w.queue(ev);
   *
   *      // ...
   *  }
   *  ```
   */
  class basic_impact {
  public:
    constexpr basic_impact() noexcept                     = default;
    constexpr basic_impact(const basic_impact &) noexcept = default;
    constexpr basic_impact(basic_impact &&) noexcept      = default;

    constexpr auto operator=(const basic_impact &) noexcept
        -> basic_impact  & = default;
    constexpr auto operator=(basic_impact &&) noexcept
        -> basic_impact  & = default;

    /*  TODO
     *  Make it contexpr after MSVC implements C++20.
     */
    virtual ~basic_impact() noexcept = default;

    constexpr void set_index(sl::index n) noexcept;

    /*  Set the order index sequence. Required for the sync
     *  Impacts to be sequentially consistent.
     */
    constexpr void set_order(cref_eventorder order) noexcept;

    /*  Set event time. If time is undefined it will be assigned
     *  by Solver due performing.
     */
    constexpr void set_time(sl::time time) noexcept;

    /*  Set actor entity id. Can be undefined. Allows to verify
     *  players' permissions.
     */
    constexpr void set_actor(sl::index id) noexcept;

    /*  TODO
     *  Make `perform` a coroutine. Add coroutine support in
     *  world and scheduler.
     */
    virtual void perform(access::world w) const noexcept;

    constexpr auto get_index() const noexcept -> sl::index;
    constexpr auto get_order() const noexcept -> cref_eventorder;
    constexpr auto get_time() const noexcept -> sl::time;
    constexpr auto get_actor() const noexcept -> sl::index;

    constexpr auto is_async() const noexcept -> bool;

    constexpr auto get_index64() const noexcept -> sl::index64;
    constexpr auto get_time64() const noexcept -> sl::time64;
    constexpr auto get_actor64() const noexcept -> sl::index64;

  protected:
    constexpr void set_async(bool is_async) noexcept;
    constexpr auto order_of_child(sl::whole &count) const noexcept
        -> eventorder;

  private:
    eventorder m_order;
    sl::time   m_time     = time_undefined;
    sl::index  m_id_actor = id_undefined;
    bool       m_is_async = true;
  };

  /*  Sequentially consistent impact.
   */
  class sync_impact : public basic_impact {
  public:
    constexpr sync_impact() noexcept;
    ~sync_impact() noexcept override = default;
  };

  /*  Impact generation functor.
   */
  using impact_gen = std::function<ptr_impact()>;

  /*  Impact generation wrapper.
   */
  template <typename basic_impact_>
  auto gen() noexcept -> impact_gen;
}

#include "basic_impact.impl.h"

#endif
