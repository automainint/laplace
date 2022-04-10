/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_ENGINE_PRIME_IMPACT_IMPL_H
#define LAPLACE_ENGINE_PRIME_IMPACT_IMPL_H

namespace laplace::engine {
  inline auto prime_impact::encode() const -> vbyte {
    auto seq = vbyte(get_encoded_size());
    encode_to(seq);
    return seq;
  }

  inline void prime_impact::encode_to(span_byte) const { }

  constexpr auto prime_impact::get_encoded_size() const -> sl::whole {
    return this->m_encoded_size;
  }

  constexpr void prime_impact::set_actor(span_byte seq,
                                         sl::index id_actor) {
    if (!is_control_id(get_id(seq))) {
      serial::wr<sl::index64>(seq, n_actor, id_actor);
    }
  }

  constexpr void prime_impact::set_index(span_byte seq, sl::index n) {
    if (!is_unindexed_id(get_id(seq)))
      serial::wr<sl::index64>(seq, n_index, n);
  }

  constexpr void prime_impact::set_time(span_byte seq,
                                        sl::time  time) {
    if (!is_control_id(get_id(seq)))
      serial::wr<sl::time64>(seq, n_time, time);
  }

  constexpr auto prime_impact::is_unindexed(span_cbyte seq) -> bool {
    return is_unindexed_id(get_id(seq));
  }

  constexpr auto prime_impact::is_control(span_cbyte seq) -> bool {
    const auto id = get_id(seq);
    return is_unindexed_id(id) || is_control_id(id);
  }

  constexpr auto prime_impact::get_index_unsafe(span_cbyte seq)
      -> sl::index {
    return as_index(serial::rd<sl::index64>(seq, n_index));
  }

  constexpr auto prime_impact::get_time_unsafe(span_cbyte seq)
      -> sl::time {
    return static_cast<sl::time>(serial::rd<sl::time64>(seq, n_time));
  }

  constexpr auto prime_impact::get_actor_unsafe(span_cbyte seq)
      -> sl::index {
    return as_index(serial::rd<sl::index64>(seq, n_actor));
  }

  constexpr auto prime_impact::get_id(span_cbyte seq) -> uint16_t {
    return serial::rd<uint16_t>(seq, n_id, protocol::ids::undefined);
  }

  constexpr auto prime_impact::get_index(span_cbyte seq)
      -> sl::index {
    if (is_unindexed_id(get_id(seq))) {
      return id_undefined;
    }

    return get_index_unsafe(seq);
  }

  constexpr auto prime_impact::get_time(span_cbyte seq) -> sl::time {
    if (is_control_id(get_id(seq)))
      return time_undefined;

    return get_time_unsafe(seq);
  }

  constexpr auto prime_impact::get_actor(span_cbyte seq)
      -> sl::index {
    if (is_control_id(get_id(seq)))
      return id_undefined;

    return get_actor_unsafe(seq);
  }

  constexpr auto prime_impact::get_intval(span_cbyte seq,
                                          sl::index  offset)
      -> intval {

    const auto value = serial::rd<int64_t>(seq, offset);
    const auto n     = static_cast<intval>(value);

    if constexpr (sizeof value != sizeof n) {
      if (static_cast<decltype(value)>(n) != value)
        return -1;
    }

    return n;
  }

  inline auto prime_impact::get_string(span_cbyte seq,
                                       sl::index  offset)
      -> std::u8string_view {

    if (offset >= seq.size())
      return {};

    return { reinterpret_cast<const char8_t *>(seq.data() + offset),
             seq.size() - offset };
  }

  inline auto prime_impact::get_string(span_cbyte seq,
                                       sl::index  offset,
                                       sl::whole  size)
      -> std::u8string_view {

    if (offset + size > seq.size())
      return {};

    return { reinterpret_cast<const char8_t *>(seq.data() + offset),
             static_cast<std::u8string_view::size_type>(size) };
  }

  constexpr void prime_impact::set_encoded_size(sl::whole size) {
    this->m_encoded_size = size;
  }

  constexpr auto prime_impact::is_unindexed_id(sl::index id) -> bool {
    using namespace protocol;

    return id == ids::request_events || id == ids::request_token ||
           id == ids::session_request ||
           id == ids::session_response || id == ids::session_token ||
           id == ids::ping_request || id == ids::ping_response ||
           id == ids::client_desync || id == ids::server_heartbeat;
  }

  constexpr auto prime_impact::is_control_id(sl::index id) -> bool {
    using namespace protocol;

    return id == ids::server_init || id == ids::server_loading ||
           id == ids::server_launch || id == ids::server_action ||
           id == ids::server_pause || id == ids::server_reserve ||
           id == ids::server_clock || id == ids::server_seed ||
           id == ids::server_quit || id == ids::client_enter ||
           id == ids::client_leave || id == ids::client_ready;
  }

  constexpr sync_prime_impact::sync_prime_impact() {
    set_async(false);
  }

  template <typename prime_impact_, typename... args_>
  inline auto encode(args_... args) -> vbyte {
    const auto ev  = prime_impact_(args...);
    auto       seq = vbyte(ev.get_encoded_size());
    ev.prime_impact_::encode_to(seq);
    return seq;
  }
}

#endif
