/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_NETWORK_INTERFACE_PROTOCOL_H
#define LAPLACE_NETWORK_INTERFACE_PROTOCOL_H

#include "../../core/static_shuffle.h"
#include "../defs.h"

namespace laplace::network {
  enum class cipher { undefined, plain, rabbit };

  static constexpr auto _id = core::static_shuffle<0, 64>(3);

  enum class control {
    undefined = _id[0],

    /*  Unindexed.
     */
    request_events   = _id[1],
    request_token    = _id[2],
    session_request  = _id[3],
    session_response = _id[4],
    session_token    = _id[5],
    ping_request     = _id[6],
    ping_response    = _id[7],
    server_heartbeat = _id[8],

    /*  Indexed.
     */
    server_reserve = _id[9],
    server_init    = _id[10],
    server_quit    = _id[11],
    server_clock   = _id[12],
    server_seed    = _id[13],
    server_action  = _id[14],
    server_pause   = _id[15],
    server_loading = _id[16],
    client_enter   = _id[17],
    client_leave   = _id[18],
    client_ready   = _id[19],

    /*  Timed events. Should not be returned by get_control_id.
     */
    slot_create = _id[20],
    slot_remove = _id[21]
  };

  struct heartbeat {
    sl::index index = 0;
    sl::time  time  = 0;
  };

  using fn_get_request_events_limit = std::function<sl::whole()>;
  using fn_get_event_index = std::function<sl::index(span_cbyte)>;
  using fn_get_event_time  = std::function<sl::time(span_cbyte)>;
  using fn_set_event_index =
      std::function<void(span_byte, sl::index)>;
  using fn_set_event_time = std::function<void(span_byte, sl::time)>;
  using fn_set_event_actor =
      std::function<void(span_byte, sl::index)>;
  using fn_is_allowed = std::function<bool(span_cbyte)>;
  using fn_alter_slot_create_flag =
      std::function<void(span_byte, sl::index)>;
  using fn_get_control_id = std::function<control(span_cbyte)>;

  using fn_decode_request_events =
      std::function<sl::vector<sl::index>(span_cbyte)>;
  using fn_decode_cipher_id  = std::function<cipher(span_cbyte)>;
  using fn_decode_public_key = std::function<span_cbyte(span_cbyte)>;
  using fn_decode_session_response_port =
      std::function<uint16_t(span_cbyte)>;
  using fn_decode_session_token =
      std::function<span_cbyte(span_cbyte)>;
  using fn_decode_ping_response = std::function<sl::time(span_cbyte)>;
  using fn_decode_server_heartbeat =
      std::function<heartbeat(span_cbyte)>;
  using fn_decode_server_clock = std::function<sl::time(span_cbyte)>;
  using fn_decode_server_seed  = std::function<uint64_t(span_cbyte)>;

  using fn_encode_request_events =
      std::function<vbyte(std::span<sl::index const>)>;
  using fn_encode_session_request =
      std::function<vbyte(cipher, span_cbyte)>;
  using fn_encode_session_response =
      std::function<vbyte(uint16_t, span_cbyte)>;
  using fn_encode_session_token    = std::function<vbyte(span_cbyte)>;
  using fn_encode_ping_request     = std::function<vbyte(sl::time)>;
  using fn_encode_ping_response    = std::function<vbyte(span_cbyte)>;
  using fn_encode_server_heartbeat = std::function<vbyte(heartbeat)>;
  using fn_encode_server_clock     = std::function<vbyte(sl::time)>;
  using fn_encode_server_seed      = std::function<vbyte(uint64_t)>;
  using fn_encode                  = std::function<vbyte(control)>;

  struct protocol_interface {
    fn_get_request_events_limit get_request_events_limit;
    fn_get_event_index          get_event_index;
    fn_get_event_time           get_event_time;
    fn_set_event_index          set_event_index;
    fn_set_event_time           set_event_time;
    fn_set_event_actor          set_event_actor;
    fn_is_allowed               is_allowed;
    fn_alter_slot_create_flag   alter_slot_create_flag;
    fn_get_control_id           get_control_id;

    fn_decode_request_events        decode_request_events;
    fn_decode_cipher_id             decode_cipher_id;
    fn_decode_public_key            decode_public_key;
    fn_decode_session_response_port decode_session_response_port;
    fn_decode_session_token         decode_session_token;
    fn_decode_ping_response         decode_ping_response;
    fn_decode_server_heartbeat      decode_server_heartbeat;
    fn_decode_server_clock          decode_server_clock;
    fn_decode_server_seed           decode_server_seed;

    fn_encode_request_events   encode_request_events;
    fn_encode_session_request  encode_session_request;
    fn_encode_session_response encode_session_response;
    fn_encode_session_token    encode_session_token;
    fn_encode_ping_request     encode_ping_request;
    fn_encode_ping_response    encode_ping_response;
    fn_encode_server_heartbeat encode_server_heartbeat;
    fn_encode_server_clock     encode_server_clock;
    fn_encode_server_seed      encode_server_seed;
    fn_encode                  encode;
  };

  [[nodiscard]] auto check_protocol_interface(
      protocol_interface const &in) noexcept -> bool;

  [[nodiscard]] auto blank_protocol_interface() noexcept
      -> protocol_interface;
}

#endif
