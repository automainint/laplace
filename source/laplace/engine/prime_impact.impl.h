/*  laplace/engine/prime_impact.impl.h
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

#ifndef laplace_engine_prime_impact_impl_h
#define laplace_engine_prime_impact_impl_h

namespace laplace::engine {
  inline auto prime_impact::encode() const -> vbyte {
    vbyte seq(get_encoded_size());
    encode_to(seq);
    return seq;
  }

  inline void prime_impact::encode_to(std::span<uint8_t>) const { }

  constexpr auto prime_impact::get_encoded_size() const -> sl::whole {
    return this->m_encoded_size;
  }

  constexpr void prime_impact::set_index(span_byte seq, sl::index n) {
    using namespace protocol;

    if (get_id(seq) >= ids::_unindexed_count)
      serial::wr<uint64_t>(seq, n_index, n);
  }

  constexpr auto prime_impact::get_id_unsafe(span_cbyte seq)
      -> uint16_t {
    return serial::rd<uint16_t>(seq, n_id);
  }

  constexpr auto prime_impact::get_index_unsafe(span_cbyte seq)
      -> sl::index {
    return as_index(serial::rd<uint64_t>(seq, n_index));
  }

  constexpr auto prime_impact::get_time_unsafe(span_cbyte seq)
      -> uint64_t {
    return serial::rd<uint64_t>(seq, n_time);
  }

  constexpr auto prime_impact::get_actor_unsafe(span_cbyte seq)
      -> sl::index {
    return as_index(serial::rd<uint64_t>(seq, n_actor));
  }

  constexpr auto prime_impact::get_id(span_cbyte seq) -> uint16_t {
    using namespace protocol;

    if (seq.size() >= sizeof(uint16_t))
      return get_id_unsafe(seq);

    return ids::undefined;
  }

  constexpr auto prime_impact::get_index(span_cbyte seq) -> sl::index {
    using namespace protocol;

    if (get_id(seq) < ids::_unindexed_count) {
      return id_undefined;
    }

    return get_index_unsafe(seq);
  }

  constexpr auto prime_impact::get_time(span_cbyte seq) -> uint64_t {
    using namespace protocol;

    if (get_id(seq) < ids::_control_count) {
      return time_undefined;
    }

    return get_time_unsafe(seq);
  }

  constexpr auto prime_impact::get_actor(span_cbyte seq) -> sl::index {
    using namespace protocol;

    if (get_id(seq) < ids::_control_count) {
      return id_undefined;
    }

    return get_actor_unsafe(seq);
  }

  inline auto prime_impact::get_string(span_cbyte seq, size_t offset)
      -> std::u8string_view {

    if (offset >= seq.size())
      return {};

    return { reinterpret_cast<const char8_t *>(seq.data() + offset),
             seq.size() - offset };
  }

  inline auto prime_impact::get_string(span_cbyte seq, size_t offset,
                                       size_t size)
      -> std::u8string_view {

    if (offset + size > seq.size())
      return {};

    return { reinterpret_cast<const char8_t *>(seq.data() + offset),
             size };
  }

  constexpr void prime_impact::set_encoded_size(sl::whole size) {
    this->m_encoded_size = size;
  }

  constexpr sync_prime_impact::sync_prime_impact() {
    set_async(false);
  }

  template <typename prime_impact_, typename... args_>
  constexpr auto encode(args_... args) -> vbyte {
    const auto ev = prime_impact_(args...);
    vbyte      seq(ev.get_encoded_size());
    ev.prime_impact_::encode_to(seq);
    return seq;
  }
}

#endif
