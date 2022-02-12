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

#include "../defs.h"

namespace laplace::network {
  enum class cipher { undefined, plain, rabbit };

  enum class control {
    undefined,
    request_events,
    request_token,
    session_request,
    session_response,
    session_token,
    ping_request,
    ping_response,
    server_heartbeat,
    server_reserve,
    server_init,
    server_quit,
    server_clock,
    server_seed,
    server_action,
    server_pause,
    server_loading,
    client_enter,
    client_leave,
    client_ready,
    slot_create,
    slot_remove
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
  using fn_is_allowed = std::function<bool(span_cbyte, bool)>;
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
