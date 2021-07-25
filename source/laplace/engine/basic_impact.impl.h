/*  laplace/engine/basic_impact.impl.h
 *
 *      Impact constexpr implementation.
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

#ifndef laplace_engine_basic_impact_impl_h
#define laplace_engine_basic_impact_impl_h

namespace laplace::engine {
  constexpr void basic_impact::set_index(sl::index n) {
    set_order({ eventorder::root + n });
  }

  constexpr void basic_impact::set_order(cref_eventorder order) {
    this->m_order = order;
  }

  constexpr void basic_impact::set_time(uint64_t time) {
    this->m_time = time;
  }

  constexpr void basic_impact::set_actor(sl::index id) {
    this->m_id_actor = id;
  }

  inline void basic_impact::perform(access::world) const { }

  constexpr auto basic_impact::get_index() const -> sl::index {
    return this->m_order.get_index() - eventorder::root;
  }

  constexpr auto basic_impact::get_order() const -> cref_eventorder {
    return this->m_order;
  }

  constexpr auto basic_impact::get_time() const -> uint64_t {
    return this->m_time;
  }

  constexpr auto basic_impact::get_actor() const -> sl::index {
    return this->m_id_actor;
  }

  constexpr auto basic_impact::is_async() const -> bool {
    return this->m_is_async;
  }

  constexpr auto basic_impact::get_index64() const -> sl::index64 {
    return static_cast<uint64_t>(this->get_index());
  }

  constexpr auto basic_impact::get_time64() const -> uint64_t {
    return static_cast<uint64_t>(this->get_time());
  }

  constexpr auto basic_impact::get_actor64() const -> sl::index64 {
    return static_cast<uint64_t>(this->get_actor());
  }

  constexpr void basic_impact::set_async(bool is_async) {
    this->m_is_async = is_async;
  }

  constexpr auto basic_impact::order_of_child(sl::whole &count) const
      -> eventorder {
    return this->m_order.spawn(count);
  }

  constexpr sync_impact::sync_impact() {
    this->set_async(false);
  }

  template <typename basic_impact_>
  inline auto gen() -> impact_gen {
    return [] {
      return std::make_shared<basic_impact_>();
    };
  }
}

#endif
