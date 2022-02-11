/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "event_interface.h"

namespace laplace::network {
  auto check_event_interface(event_interface const &in) noexcept
      -> bool {
    return in.is_desync && in.get_current_time &&
           in.get_request_events_limit && in.get_event_index &&
           in.get_event_time && in.set_event_index &&
           in.set_event_time && in.set_event_actor && in.is_allowed &&
           in.alter_slot_create_flag && in.decode_request_events &&
           in.decode_server_clock && in.encode_request_events &&
           in.decode_ping_response && in.encode_session_token &&
           in.encode_ping_request && in.encode_ping_response &&
           in.encode_server_idle && in.encode_server_quit &&
           in.encode_client_leave && in.encode_slot_remove &&
           in.scan_request_events && in.scan_request_token &&
           in.scan_session_request && in.scan_session_response &&
           in.scan_session_token && in.scan_ping_request &&
           in.scan_ping_response && in.scan_server_idle &&
           in.scan_server_init && in.scan_server_loading &&
           in.scan_server_action && in.scan_server_pause &&
           in.scan_server_reserve && in.scan_server_clock &&
           in.scan_server_seed && in.scan_server_quit &&
           in.scan_client_enter && in.scan_client_leave &&
           in.scan_client_ready && in.scan_slot_create &&
           in.scan_slot_remove && in.do_server_seed &&
           in.do_server_reserve && in.do_wait_loading &&
           in.do_loading && in.do_apply && in.do_perform &&
           in.do_schedule && in.do_cleanup;
  }

  auto blank_event_interface() noexcept -> event_interface {
    auto const blank_scan   = [](span_cbyte) { return false; };
    auto const blank_action = [](span_cbyte) {};

    return {
      .is_desync = []() -> bool { return false; },
      .get_current_time = []() -> sl::time { return 0; },
      .get_request_events_limit = []() -> sl::whole { return 0; },
      .get_event_index = [](span_cbyte) -> sl::index { return -1; },
      .get_event_time = [](span_cbyte) -> sl::time { return -1; },
      .set_event_index = [](span_byte, sl::index) {},
      .set_event_time  = [](span_byte, sl::index) {},
      .set_event_actor = [](span_byte, sl::index) {},
      .is_allowed      = [](span_cbyte, bool) { return false; },
      .alter_slot_create_flag = [](span_byte, sl::index) {},
      .decode_request_events =
          [](span_cbyte) -> sl::vector<sl::index> { return {}; },
      .decode_ping_response = [](span_cbyte) -> sl::time {
        return 0;
      },
      .decode_server_clock = [](span_cbyte) -> sl::time { return 1; },
      .encode_request_events =
          [](std::span<sl::index const>) -> vbyte { return {}; },
      .encode_session_token = [](span_cbyte) -> vbyte { return {}; },
      .encode_ping_request = [](sl::time) -> vbyte { return {}; },
      .encode_ping_response = [](span_cbyte) -> vbyte { return {}; },
      .encode_server_idle = [](sl::index) -> vbyte { return {}; },
      .encode_server_quit = []() -> vbyte { return {}; },
      .encode_client_leave = []() -> vbyte { return {}; },
      .encode_slot_remove = []() -> vbyte { return {}; },
      .scan_request_events   = blank_scan,
      .scan_request_token    = blank_scan,
      .scan_session_request  = blank_scan,
      .scan_session_response = blank_scan,
      .scan_session_token    = blank_scan,
      .scan_ping_request     = blank_scan,
      .scan_ping_response    = blank_scan,
      .scan_server_idle      = blank_scan,
      .scan_server_init      = blank_scan,
      .scan_server_loading   = blank_scan,
      .scan_server_action    = blank_scan,
      .scan_server_pause     = blank_scan,
      .scan_server_reserve   = blank_scan,
      .scan_server_clock     = blank_scan,
      .scan_server_seed      = blank_scan,
      .scan_server_quit      = blank_scan,
      .scan_client_enter     = blank_scan,
      .scan_client_leave     = blank_scan,
      .scan_client_ready     = blank_scan,
      .do_server_seed        = blank_action,
      .do_server_reserve     = blank_action,
      .do_wait_loading       = []() -> bool { return false; },
      .do_loading = [](span_cbyte) {},
      .do_apply   = [](span_cbyte) -> sl::time { return 0; },
      .do_perform  = [](span_cbyte) {},
      .do_schedule = [](sl::time) {},
      .do_cleanup  = []() {}
    };
  }
}
