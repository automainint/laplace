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

#ifndef __laplace__engine_basic_impact_impl__
#define __laplace__engine_basic_impact_impl__

namespace laplace::engine {
  constexpr void
  basic_impact::set_order(cref_eventorder order) {
    this->m_order = order;
  }

  constexpr void basic_impact::set_time(uint64_t time) {
    this->m_time = time;
  }

  constexpr void basic_impact::set_actor(size_t id) {
    this->m_id_actor = id;
  }

  inline void basic_impact::perform(access::world w) const { }

  inline auto basic_impact::encode() const -> laplace::vbyte {
    vbyte seq(get_encoded_size());
    encode_to(seq);
    return seq;
  }

  inline void
  basic_impact::encode_to(std::span<uint8_t> bytes) const { }

  constexpr auto basic_impact::get_encoded_size() const
      -> size_t {
    return this->m_size;
  }

  constexpr auto basic_impact::get_index() const -> size_t {
    return this->m_order.get_index();
  }

  constexpr auto basic_impact::get_order() const
      -> cref_eventorder {
    return this->m_order;
  }

  constexpr auto basic_impact::get_time() const -> uint64_t {
    return this->m_time;
  }

  constexpr auto basic_impact::get_actor() const -> size_t {
    return this->m_id_actor;
  }

  constexpr auto basic_impact::is_async() const -> bool {
    return this->m_is_async;
  }

  constexpr auto basic_impact::get_index64() const -> uint64_t {
    return static_cast<uint64_t>(this->get_index());
  }

  constexpr auto basic_impact::get_time64() const -> uint64_t {
    return static_cast<uint64_t>(this->get_time());
  }

  constexpr auto basic_impact::get_actor64() const -> uint64_t {
    return static_cast<uint64_t>(this->get_actor());
  }

  constexpr void basic_impact::set_index(ref_vbyte seq,
                                         size_t    index) {
    using namespace protocol;

    if (basic_impact::get_id(seq) >= ids::_unindexed_count)
      wr<uint64_t>(seq, basic_impact::n_index, index);
  }

  constexpr auto basic_impact::get_id_unsafe(cref_vbyte seq)
      -> uint16_t {
    return rd<uint16_t>(seq, basic_impact::n_id);
  }

  constexpr auto basic_impact::get_index_unsafe(cref_vbyte seq)
      -> size_t {
    return as_index(rd<uint64_t>(seq, basic_impact::n_index));
  }

  constexpr auto basic_impact::get_time_unsafe(cref_vbyte seq)
      -> uint64_t {
    return rd<uint64_t>(seq, basic_impact::n_time);
  }

  constexpr auto basic_impact::get_actor_unsafe(cref_vbyte seq)
      -> size_t {
    return as_index(rd<uint64_t>(seq, basic_impact::n_actor));
  }

  constexpr auto basic_impact::get_id(cref_vbyte seq)
      -> uint16_t {
    using namespace protocol;

    return seq.size() >= sizeof(uint16_t) ? get_id_unsafe(seq)
                                          : ids::undefined;
  }

  constexpr auto basic_impact::get_index(cref_vbyte seq)
      -> size_t {
    using namespace protocol;

    if (basic_impact::get_id(seq) < ids::_unindexed_count) {
      return -1;
    }

    return get_index_unsafe(seq);
  }

  constexpr auto basic_impact::get_time(cref_vbyte seq)
      -> uint64_t {
    using namespace protocol;

    if (basic_impact::get_id(seq) < ids::_control_count) {
      return time_undefined;
    }

    return get_time_unsafe(seq);
  }

  constexpr auto basic_impact::get_actor(cref_vbyte seq)
      -> size_t {
    using namespace protocol;

    if (basic_impact::get_id(seq) < ids::_control_count) {
      return id_undefined;
    }

    return get_actor_unsafe(seq);
  }

  inline auto basic_impact::get_string(cref_vbyte seq,
                                       size_t     offset)
      -> std::u8string_view {
    if (offset >= seq.size())
      return {};

    return std::u8string_view {
      reinterpret_cast<const char8_t *>(seq.data() + offset),
      seq.size() - offset
    };
  }

  inline auto basic_impact::get_string(cref_vbyte seq,
                                       size_t     offset,
                                       size_t     size)
      -> std::u8string_view {
    if (offset + size > seq.size())
      return {};

    return std::u8string_view {
      reinterpret_cast<const char8_t *>(seq.data() + offset),
      size
    };
  }

  constexpr void basic_impact::set_async(bool is_async) {
    this->m_is_async = is_async;
  }

  constexpr void basic_impact::set_size(size_t size) {
    this->m_size = size;
  }

  constexpr auto
  basic_impact::order_of_child(ref_uint count) const
      -> eventorder {
    return this->m_order.spawn(count);
  }

  constexpr sync_impact::sync_impact() {
    this->set_async(false);
  }

  template <typename impact_>
  constexpr auto encode(const impact_ &ev) -> vbyte {
    vbyte seq(ev.get_encoded_size());
    ev.impact_::encode_to(seq);
    return seq;
  }

  template <typename basic_impact_type>
  inline auto gen() -> impact_gen {
    using namespace std;

    return []() -> ptr_impact {
      return make_shared<basic_impact_type>();
    };
  }
}

#endif
