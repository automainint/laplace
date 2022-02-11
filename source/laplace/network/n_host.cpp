/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "host.h"

#include "../core/string.h"
#include "crypto/ecc_rabbit.h"

namespace laplace::network {
  using std::make_unique, crypto::ecc_rabbit;

  host::host() {
    auto dev = std::random_device {};
    m_rand.seed(dev());
    m_seed = dev();

    set_max_slot_count(slot_count_unlimited);
    set_master(true);
  }

  void host::listen(uint16_t port) {
    cleanup();
    set_connected(true);

    m_node = m_socket_interface->open(port);

    process_event(slot_host, m_proto.encode_server_clock(
                                 m_clock.get_tick_duration()));
    process_event(slot_host, m_proto.encode_server_seed(m_seed));
    process_event(slot_host, m_proto.encode_server_init());

    perform_instant_events();
  }

  auto host::perform_control(sl::index slot, span_cbyte seq)
      -> event_status {

    if (m_proto.scan_session_request(seq)) {
      if (slot < 0 || slot >= m_slots.size()) {
        error_("Invalid slot.", __FUNCTION__);
        return event_status::remove;
      }

      m_slots[slot].token = generate_token();
      m_slots[slot].node  = m_socket_interface->open(any_port);

      if (m_proto.scan_cipher(cipher::rabbit, seq)) {
        m_slots[slot].tran.setup_cipher<ecc_rabbit>();
        m_slots[slot].tran.set_remote_key(
            m_proto.decode_public_key(seq));

        send_event_to(slot, m_proto.encode_session_response(
                                m_slots[slot].node->get_port(),
                                m_slots[slot].tran.get_public_key()));
      } else {
        if (m_proto.scan_cipher(cipher::plain, seq))
          m_log.print(fmt("Unknown cipher on slot %d.", (int) slot));

        send_event_to(
            slot, m_proto.encode_session_response(
                      m_slots[slot].node->get_port(), span_cbyte {}));
      }

      return event_status::remove;
    }

    if (m_proto.scan_session_token(seq)) {
      if (slot < 0 || slot >= m_slots.size()) {
        error_("Invalid slot.", __FUNCTION__);
        return event_status::remove;
      }

      auto const token = m_proto.decode_session_token(seq);

      if (token.empty())
        return event_status::remove;

      for (sl::index i = 0; i < m_slots.size(); i++) {
        auto &s = m_slots[i];

        if (i == slot || s.token.empty())
          continue;

        bool const equal = [&]() {
          if (s.token.size() != token.size())
            return false;
          for (sl::index k = 0; k < token.size(); k++)
            if (s.token[k] != token[k])
              return false;
          return true;
        }();

        if (!equal)
          continue;

        if (m_slots[slot].id_actor != id_undefined)
          process_event(slot, m_proto.encode_slot_remove());

        m_slots[slot].token    = s.token;
        m_slots[slot].id_actor = s.id_actor;

        s.id_actor     = id_undefined;
        s.is_connected = false;

        break;
      }

      return event_status::remove;
    }

    if (m_proto.scan_client_enter(seq)) {
      if (slot < 0 || slot >= m_slots.size()) {
        error_("Invalid slot.", __FUNCTION__);
        return event_status::remove;
      }

      if (m_slots[slot].id_actor == id_undefined) {
        m_slots[slot].id_actor = m_exe.do_reserve();
        process_event(slot, m_proto.encode_slot_create());
      }

      return event_status::remove;
    }

    if (m_proto.scan_client_leave(seq)) {
      if (slot < 0 || slot >= m_slots.size()) {
        error_("Invalid slot.", __FUNCTION__);
        return event_status::remove;
      }

      if (get_state() == server_state::prepare) {
        process_event(slot, m_proto.encode_slot_remove());
        m_slots[slot].id_actor = id_undefined;
      }

      m_slots[slot].is_connected = false;
      return event_status::remove;
    }

    if (m_proto.scan_client_ready(seq)) {
      if (slot < 0 || slot >= m_slots.size()) {
        error_("Invalid slot.", __FUNCTION__);
        return event_status::remove;
      }

      if (get_state() == server_state::prepare)
        add_instant_event(seq);
      else
        m_log.print("Ignore ready command.");

      return event_status::remove;
    }

    return udp_server::perform_control(slot, seq);
  }

  auto host::generate_token() -> vbyte {
    auto token = vbyte(256);
    auto dist  = std::uniform_int_distribution<int>(0, 255);

    for (auto &byte : token)
      byte = static_cast<uint8_t>(dist(m_rand));

    return token;
  }
}
