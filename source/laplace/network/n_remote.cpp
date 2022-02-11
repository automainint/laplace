/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "remote.h"

#include "crypto/ecc_rabbit.h"

namespace laplace::network {
  using std::string, std::string_view, crypto::ecc_rabbit;

  remote::remote() {
    set_master(false);
  }

  void remote::reconnect() {
    cleanup();

    set_state(server_state::prepare);
    set_connected(true);

    m_node = m_socket_interface->open(m_client_port);

    auto slot = add_slot(m_host_address, m_host_port);

    if (is_encryption_enabled()) {
      m_slots[slot].tran.setup_cipher<ecc_rabbit>();

      send_event_to(slot, m_proto.encode_session_request(
                              cipher::rabbit,
                              m_slots[slot].tran.get_public_key()));
    } else {
      send_event_to(
          slot, m_proto.encode_session_request(cipher::plain, {}));
    }

    queue(m_proto.encode_client_enter());
  }

  void remote::connect(string_view host_address, uint16_t host_port,
                       uint16_t client_port) {

    m_host_address = host_address;
    m_host_port    = host_port;
    m_client_port  = client_port;

    reconnect();
  }

  auto remote::perform_control(sl::index slot, span_cbyte seq)
      -> event_status {

    if (m_proto.scan_session_response(seq)) {
      if (slot < 0 || slot >= m_slots.size()) {
        error_("Invalid slot.", __FUNCTION__);
        return event_status::remove;
      }

      auto const key = m_proto.decode_public_key(seq);

      if (!key.empty()) {
        m_slots[slot].tran.set_remote_key(key);

        m_slots[slot].encryption = m_slots[slot].tran.is_encrypted()
                                       ? transfer::encrypted
                                       : transfer::plain;
      }

      m_slots[slot].node = m_socket_interface->open(any_port);
      m_slots[slot].port = m_proto.decode_session_response_port(seq);
      m_slots[slot].is_exclusive = true;

      if (m_token.empty())
        send_event_to(slot, m_proto.encode_request_token());
      else
        send_event_to(slot, m_proto.encode_session_token(m_token));

      return event_status::remove;
    }

    if (m_proto.scan_session_token(seq)) {
      if (slot < 0 || slot >= m_slots.size()) {
        error_("Invalid slot.", __FUNCTION__);
        return event_status::remove;
      }

      auto const token = m_proto.decode_session_token(seq);

      m_token.assign(token.begin(), token.end());

      return event_status::remove;
    }

    if (m_proto.scan_server_heartbeat(seq)) {
      if (slot < 0 || slot >= m_slots.size()) {
        error_("Invalid slot.", __FUNCTION__);
        return event_status::remove;
      }

      auto const [index, time] = m_proto.decode_server_heartbeat(seq);

      if (index >= 0) {
        auto const &qu          = m_slots[slot].queue;
        auto const  event_count = qu.index + qu.events.size();

        if (index > event_count) {
          auto events = sl::vector<sl::index> {};

          for (sl::index n = event_count; n < index; n++) {
            events.emplace_back(n);
            if (events.size() >= m_proto.get_request_events_limit())
              break;
          }

          send_event_to(slot, m_proto.encode_request_events(events));
        }

        update_time_limit(time);
      }

      return event_status::remove;
    }

    if (m_proto.scan_server_quit(seq)) {
      if (slot < 0 || slot >= m_slots.size()) {
        error_("Invalid slot.", __FUNCTION__);
        return event_status::remove;
      }

      set_connected(false);
      set_quit(true);
      m_node.reset();

      return event_status::remove;
    }

    return udp_server::perform_control(slot, seq);
  }
}
