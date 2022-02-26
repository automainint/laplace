/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "protocol.h"

namespace laplace::network {
  auto check_protocol_interface(protocol_interface const &in) noexcept
      -> bool {
    return in.get_request_events_limit && in.get_event_index &&
           in.get_event_time && in.set_event_index &&
           in.set_event_time && in.set_event_actor && in.is_allowed &&
           in.alter_slot_create_flag && in.get_control_id &&

           in.decode_request_events && in.decode_cipher_id &&
           in.decode_public_key && in.decode_session_response_port &&
           in.decode_session_token && in.decode_ping_response &&
           in.decode_server_heartbeat && in.decode_server_clock &&
           in.decode_server_seed &&

           in.encode_request_events && in.encode_session_request &&
           in.encode_session_response && in.encode_session_token &&
           in.encode_ping_request && in.encode_ping_response &&
           in.encode_server_heartbeat && in.encode_server_clock &&
           in.encode_server_seed && in.encode;
  }

  auto blank_protocol_interface() noexcept -> protocol_interface {
    return {
      .get_request_events_limit = []() -> sl::whole { return 0; },
      .get_event_index = [](span_cbyte) -> sl::index { return -1; },
      .get_event_time  = [](span_cbyte) -> sl::time { return -1; },
      .set_event_index = [](span_byte, sl::index) {},
      .set_event_time  = [](span_byte, sl::index) {},
      .set_event_actor = [](span_byte, sl::index) {},
      .is_allowed      = [](span_cbyte) { return false; },
      .alter_slot_create_flag = [](span_byte, sl::index) {},
      .get_control_id         = [](span_cbyte) -> control {
        return control::undefined;
      },

      .decode_request_events =
          [](span_cbyte) -> sl::vector<sl::index> { return {}; },
      .decode_cipher_id = [](span_cbyte) -> cipher {
        return cipher::undefined;
      },
      .decode_public_key = [](span_cbyte) -> span_cbyte {
        return {};
      },
      .decode_session_response_port = [](span_cbyte) -> uint16_t {
        return 0;
      },
      .decode_session_token = [](span_cbyte) -> span_cbyte {
        return {};
      },
      .decode_ping_response = [](span_cbyte) -> sl::time {
        return 0;
      },
      .decode_server_heartbeat = [](span_cbyte) -> heartbeat {
        return {};
      },
      .decode_server_clock = [](span_cbyte) -> sl::time { return 1; },
      .decode_server_seed  = [](span_cbyte) -> uint64_t { return 0; },

      .encode_request_events =
          [](std::span<sl::index const>) -> vbyte { return {}; },
      .encode_session_request = [](cipher, span_cbyte) -> vbyte {
        return {};
      },
      .encode_session_response = [](uint16_t, span_cbyte) -> vbyte {
        return {};
      },
      .encode_session_token = [](span_cbyte) -> vbyte { return {}; },
      .encode_ping_request  = [](sl::time) -> vbyte { return {}; },
      .encode_ping_response = [](span_cbyte) -> vbyte { return {}; },
      .encode_server_heartbeat = [](heartbeat) -> vbyte {
        return {};
      },
      .encode_server_clock = [](sl::time) -> vbyte { return {}; },
      .encode_server_seed  = [](uint64_t) -> vbyte { return {}; },
      .encode              = [](control) -> vbyte { return {}; }
    };
  }
}
