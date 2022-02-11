/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_network_event_interface_h
#define laplace_network_event_interface_h

#include "defs.h"

namespace laplace::network {
  using fn_is_desync                = std::function<bool()>;
  using fn_get_current_time         = std::function<sl::time()>;
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

  using fn_decode_request_events =
      std::function<sl::vector<sl::index>(span_cbyte)>;
  using fn_decode_ping_response = std::function<sl::time(span_cbyte)>;
  using fn_decode_server_clock  = std::function<sl::time(span_cbyte)>;

  using fn_encode_request_events =
      std::function<vbyte(std::span<sl::index const>)>;
  using fn_encode_session_token = std::function<vbyte(span_cbyte)>;
  using fn_encode_ping_request  = std::function<vbyte(sl::time)>;
  using fn_encode_ping_response = std::function<vbyte(span_cbyte)>;
  using fn_encode_server_idle   = std::function<vbyte(sl::index)>;
  using fn_encode_server_quit   = std::function<vbyte()>;
  using fn_encode_client_leave  = std::function<vbyte()>;
  using fn_encode_slot_remove   = std::function<vbyte()>;

  using fn_scan_request_events   = std::function<bool(span_cbyte)>;
  using fn_scan_request_token    = std::function<bool(span_cbyte)>;
  using fn_scan_session_request  = std::function<bool(span_cbyte)>;
  using fn_scan_session_response = std::function<bool(span_cbyte)>;
  using fn_scan_session_token    = std::function<bool(span_cbyte)>;
  using fn_scan_ping_request     = std::function<bool(span_cbyte)>;
  using fn_scan_ping_response    = std::function<bool(span_cbyte)>;
  using fn_scan_server_idle      = std::function<bool(span_cbyte)>;
  using fn_scan_server_init      = std::function<bool(span_cbyte)>;
  using fn_scan_server_loading   = std::function<bool(span_cbyte)>;
  using fn_scan_server_action    = std::function<bool(span_cbyte)>;
  using fn_scan_server_pause     = std::function<bool(span_cbyte)>;
  using fn_scan_server_reserve   = std::function<bool(span_cbyte)>;
  using fn_scan_server_clock     = std::function<bool(span_cbyte)>;
  using fn_scan_server_seed      = std::function<bool(span_cbyte)>;
  using fn_scan_server_quit      = std::function<bool(span_cbyte)>;
  using fn_scan_client_enter     = std::function<bool(span_cbyte)>;
  using fn_scan_client_leave     = std::function<bool(span_cbyte)>;
  using fn_scan_client_ready     = std::function<bool(span_cbyte)>;
  using fn_scan_slot_create      = std::function<bool(span_cbyte)>;
  using fn_scan_slot_remove      = std::function<bool(span_cbyte)>;

  using fn_do_server_seed    = std::function<void(span_cbyte)>;
  using fn_do_server_reserve = std::function<void(span_cbyte)>;
  using fn_do_wait_loading   = std::function<bool()>;
  using fn_do_loading        = std::function<void(span_cbyte)>;
  using fn_do_apply          = std::function<sl::time(span_cbyte)>;
  using fn_do_perform        = std::function<void(span_cbyte)>;
  using fn_do_schedule       = std::function<void(sl::time)>;
  using fn_do_cleanup        = std::function<void()>;

  struct event_interface {
    fn_is_desync                is_desync;
    fn_get_current_time         get_current_time;
    fn_get_request_events_limit get_request_events_limit;
    fn_get_event_index          get_event_index;
    fn_get_event_time           get_event_time;
    fn_set_event_index          set_event_index;
    fn_set_event_time           set_event_time;
    fn_set_event_actor          set_event_actor;
    fn_is_allowed               is_allowed;
    fn_alter_slot_create_flag   alter_slot_create_flag;
    fn_decode_request_events    decode_request_events;
    fn_decode_ping_response     decode_ping_response;
    fn_decode_server_clock      decode_server_clock;
    fn_encode_request_events    encode_request_events;
    fn_encode_session_token     encode_session_token;
    fn_encode_ping_request      encode_ping_request;
    fn_encode_ping_response     encode_ping_response;
    fn_encode_server_idle       encode_server_idle;
    fn_encode_server_quit       encode_server_quit;
    fn_encode_client_leave      encode_client_leave;
    fn_encode_slot_remove       encode_slot_remove;
    fn_scan_request_events      scan_request_events;
    fn_scan_request_token       scan_request_token;
    fn_scan_session_request     scan_session_request;
    fn_scan_session_response    scan_session_response;
    fn_scan_session_token       scan_session_token;
    fn_scan_ping_request        scan_ping_request;
    fn_scan_ping_response       scan_ping_response;
    fn_scan_server_idle         scan_server_idle;
    fn_scan_server_init         scan_server_init;
    fn_scan_server_loading      scan_server_loading;
    fn_scan_server_action       scan_server_action;
    fn_scan_server_pause        scan_server_pause;
    fn_scan_server_reserve      scan_server_reserve;
    fn_scan_server_clock        scan_server_clock;
    fn_scan_server_seed         scan_server_seed;
    fn_scan_server_quit         scan_server_quit;
    fn_scan_client_enter        scan_client_enter;
    fn_scan_client_leave        scan_client_leave;
    fn_scan_client_ready        scan_client_ready;
    fn_scan_slot_create         scan_slot_create;
    fn_scan_slot_remove         scan_slot_remove;
    fn_do_server_seed           do_server_seed;
    fn_do_server_reserve        do_server_reserve;
    fn_do_wait_loading          do_wait_loading;
    fn_do_loading               do_loading;
    fn_do_apply                 do_apply;
    fn_do_perform               do_perform;
    fn_do_schedule              do_schedule;
    fn_do_cleanup               do_cleanup;
  };

  [[nodiscard]] auto check_event_interface(
      event_interface const &in) noexcept -> bool;

  [[nodiscard]] auto blank_event_interface() noexcept
      -> event_interface;
}

#endif
