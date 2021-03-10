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

  constexpr auto prime_impact::get_encoded_size() const -> size_t {
    return this->m_encoded_size;
  }

  constexpr void prime_impact::set_index(ref_vbyte seq, size_t index) {
    using namespace protocol;

    if (get_id(seq) >= ids::_unindexed_count)
      wr<uint64_t>(seq, n_index, index);
  }

  constexpr auto prime_impact::get_id_unsafe(cref_vbyte seq)
      -> uint16_t {
    return rd<uint16_t>(seq, n_id);
  }

  constexpr auto prime_impact::get_index_unsafe(cref_vbyte seq)
      -> size_t {
    return as_index(rd<uint64_t>(seq, n_index));
  }

  constexpr auto prime_impact::get_time_unsafe(cref_vbyte seq)
      -> uint64_t {
    return rd<uint64_t>(seq, n_time);
  }

  constexpr auto prime_impact::get_actor_unsafe(cref_vbyte seq)
      -> size_t {
    return as_index(rd<uint64_t>(seq, n_actor));
  }

  constexpr auto prime_impact::get_id(cref_vbyte seq) -> uint16_t {
    using namespace protocol;

    return seq.size() >= sizeof(uint16_t) ? get_id_unsafe(seq)
                                          : ids::undefined;
  }

  constexpr auto prime_impact::get_index(cref_vbyte seq) -> size_t {
    using namespace protocol;

    if (get_id(seq) < ids::_unindexed_count) {
      return id_undefined;
    }

    return get_index_unsafe(seq);
  }

  constexpr auto prime_impact::get_time(cref_vbyte seq) -> uint64_t {
    using namespace protocol;

    if (get_id(seq) < ids::_control_count) {
      return time_undefined;
    }

    return get_time_unsafe(seq);
  }

  constexpr auto prime_impact::get_actor(cref_vbyte seq) -> size_t {
    using namespace protocol;

    if (get_id(seq) < ids::_control_count) {
      return id_undefined;
    }

    return get_actor_unsafe(seq);
  }

  inline auto prime_impact::get_string(cref_vbyte seq, size_t offset)
      -> std::u8string_view {

    if (offset >= seq.size())
      return {};

    return { reinterpret_cast<const char8_t *>(seq.data() + offset),
             seq.size() - offset };
  }

  inline auto prime_impact::get_string(cref_vbyte seq, size_t offset,
                                       size_t size)
      -> std::u8string_view {

    if (offset + size > seq.size())
      return {};

    return { reinterpret_cast<const char8_t *>(seq.data() + offset),
             size };
  }

  constexpr void prime_impact::set_encoded_size(size_t size) {
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
