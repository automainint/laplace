/*  laplace/engine/basic_impact.h
 *
 *      Basic constexpr class for any gameplay event.
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

#ifndef laplace_engine_basic_impact_h
#define laplace_engine_basic_impact_h

#include "../core/utils.h"
#include "access/world.h"
#include "eventorder.h"
#include "protocol/ids.h"

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
   *      size_t child_count = 0;
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
    constexpr basic_impact()                     = default;
    constexpr basic_impact(const basic_impact &) = default;
    constexpr basic_impact(basic_impact &&)      = default;

    constexpr auto operator=(const basic_impact &)
        -> basic_impact &  = default;
    constexpr auto operator=(basic_impact &&)
        -> basic_impact &  = default;

    /*  TODO
     *  Make it contexpr after MSVC implements C++20.
     */
    virtual ~basic_impact() = default;

    constexpr void set_index(sl::index n);

    /*  Set the order index sequence. Required for the sync
     *  Impacts to be sequentially consistent.
     */
    constexpr void set_order(cref_eventorder order);

    /*  Set event time. If time is undefined it will be assigned
     *  by Solver due performing.
     */
    constexpr void set_time(uint64_t time);

    /*  Set actor entity id. Can be undefined. Allows to verify
     *  players' permissions.
     */
    constexpr void set_actor(sl::index id);

    virtual void perform(access::world w) const;

    constexpr auto get_index() const -> sl::index;
    constexpr auto get_order() const -> cref_eventorder;
    constexpr auto get_time() const -> uint64_t;
    constexpr auto get_actor() const -> sl::index;

    constexpr auto is_async() const -> bool;

    constexpr auto get_index64() const -> sl::index64;
    constexpr auto get_time64() const -> uint64_t;
    constexpr auto get_actor64() const -> sl::index64;

  protected:
    constexpr void set_async(bool is_async);
    constexpr auto order_of_child(sl::whole &count) const -> eventorder;

  private:
    eventorder m_order;
    uint64_t   m_time     = time_undefined;
    sl::index  m_id_actor = id_undefined;
    bool       m_is_async = true;
  };

  /*  Sequentially consistent impact.
   */
  class sync_impact : public basic_impact {
  public:
    constexpr sync_impact();
    ~sync_impact() override = default;
  };

  /*  Impact generation functor.
   */
  using impact_gen = std::function<ptr_impact()>;

  /*  Impact generation wrapper.
   */
  template <typename basic_impact_>
  auto gen() -> impact_gen;
}

#include "basic_impact.impl.h"

#endif
