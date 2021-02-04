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

#ifndef __laplace__engine_basic_impact__
#define __laplace__engine_basic_impact__

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
   *      size_t childs_count = 0;
   *      // ...
   *
   *      //  Create an Impact.
   *      auto ev     = make_shared<my_new_impact>();
   *
   *      //  Calculate the order index sequence.
   *      auto order  = order_of_child(childs_count);
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
    enum encoding_offset : size_t {
      n_id    = 0,
      n_index = 2,
      n_time  = 10,
      n_actor = 18
    };

    /*  Max encoded basic_impact size.
     */
    static constexpr size_t max_size =
        std::numeric_limits<uint16_t>::max();

    constexpr basic_impact()                     = default;
    constexpr basic_impact(const basic_impact &) = default;
    constexpr basic_impact(basic_impact &&)      = default;

    constexpr basic_impact &
    operator=(const basic_impact &) = default;
    constexpr basic_impact &
    operator=(basic_impact &&) = default;

    /*  TODO
     *  Make it contexpr after MSVC implements C++20.
     */
    virtual ~basic_impact() = default;

    /*  Set the order index sequence. Required
     *  for the sync basic_impacts to be sequentially
     *  consistent.
     */
    constexpr void set_order(cref_eventorder order);

    /*  Set event time. If time is undefined
     *  it will be assigned by Solver due
     *  performing.
     */
    constexpr void set_time(uint64_t time);

    /*  Set actor entity id. Can be undefined. Allows
     *  to verify players' permissions.
     */
    constexpr void set_actor(size_t id);

    virtual void perform(access::world w) const;

    /*  Encode into byte sequence. The sequence
     *  can be decoded by the Impact Factory.
     */
    auto encode() const -> vbyte;

    /*  TODO
     *  Make it constexpr.
     */
    virtual void encode_to(std::span<uint8_t> bytes) const;

    constexpr auto get_encoded_size() const -> size_t;

    constexpr auto get_index() const -> size_t;
    constexpr auto get_order() const -> cref_eventorder;
    constexpr auto get_time() const -> uint64_t;
    constexpr auto get_actor() const -> size_t;

    constexpr auto is_async() const -> bool;

    constexpr auto get_index64() const -> uint64_t;
    constexpr auto get_time64() const -> uint64_t;
    constexpr auto get_actor64() const -> uint64_t;

    static constexpr void set_index(ref_vbyte seq,
                                    size_t    index);

    static constexpr auto get_id_unsafe(cref_vbyte seq)
        -> uint16_t;
    static constexpr auto get_index_unsafe(cref_vbyte seq)
        -> size_t;
    static constexpr auto get_time_unsafe(cref_vbyte seq)
        -> uint64_t;
    static constexpr auto get_actor_unsafe(cref_vbyte seq)
        -> size_t;

    static constexpr auto get_id(cref_vbyte seq) -> uint16_t;
    static constexpr auto get_index(cref_vbyte seq) -> size_t;
    static constexpr auto get_time(cref_vbyte seq) -> uint64_t;
    static constexpr auto get_actor(cref_vbyte seq) -> size_t;

    static inline auto get_string(cref_vbyte seq, size_t offset)
        -> std::u8string_view;

    static inline auto get_string(cref_vbyte seq, size_t offset,
                                  size_t size)
        -> std::u8string_view;

  protected:
    constexpr void set_async(bool is_async);
    constexpr void set_size(size_t size);
    constexpr auto order_of_child(ref_uint count) const
        -> eventorder;

  private:
    eventorder m_order;
    uint64_t   m_time     = time_undefined;
    size_t     m_id_actor = id_undefined;
    bool       m_is_async = true;
    size_t     m_size     = 0;
  };

  /*  Sequentially consistent basic_impact.
   */
  class sync_impact : public basic_impact {
  public:
    constexpr sync_impact();
    ~sync_impact() override = default;
  };

  /*  Impact generation functor.
   */
  using impact_gen = std::function<ptr_impact()>;

  template <typename impact_>
  static constexpr auto encode(const impact_ &ev) -> vbyte;

  /*  Impact generation wrapper.
   */
  template <typename basic_impact_type>
  auto gen() -> impact_gen;
}

#include "basic_impact.impl.h"

#endif
