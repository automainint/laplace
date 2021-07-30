/*  laplace/network/n_remote.cpp
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

#include "remote.h"

#include "../engine/protocol/all.h"
#include "crypto/ecc_rabbit.h"

namespace laplace::network {
  namespace access = engine::access;
  using namespace engine::protocol;

  using std::min, std::make_unique, std::string, std::string_view,
      engine::encode, engine::prime_impact, engine::time_undefined,
      crypto::ecc_rabbit;

  remote::remote() {
    setup_world();

    set_master(false);
  }

  void remote::reconnect() {
    cleanup();

    if (auto wor = get_world(); wor) {
      wor->clear();
    }

    if (auto sol = get_solver(); sol) {
      sol->clear_history();
    }

    set_state(server_state::prepare);
    set_connected(true);

    m_node = make_unique<udp_node>(m_client_port);

    auto slot = add_slot(m_host_address, m_host_port);

    if (is_encryption_enabled()) {
      m_slots[slot].tran.setup_cipher<ecc_rabbit>();

      send_event_to(slot, encode<session_request>(
                              ids::cipher_ecc_rabbit,
                              m_slots[slot].tran.get_public_key()));
    } else {

      send_event_to(slot, encode<session_request>(ids::cipher_plain,
                                                  span_cbyte {}));
    }

    emit<client_enter>();
  }

  void remote::connect(string_view host_address, uint16_t host_port,
                       uint16_t client_port) {

    m_host_address = host_address;
    m_host_port    = host_port;
    m_client_port  = client_port;

    reconnect();
  }

  auto remote::perform_control(sl::index slot, span_cbyte seq) -> bool {

    if (session_response::scan(seq)) {
      if (slot < 0 || slot >= m_slots.size()) {
        error_("Invalid slot.", __FUNCTION__);
        return true;
      }

      const auto key = session_response::get_key(seq);

      if (!key.empty()) {
        m_slots[slot].tran.set_remote_key(key);
        m_slots[slot].is_encrypted = m_slots[slot].tran.is_encrypted();
      }

      m_slots[slot].node         = make_unique<udp_node>(any_port);
      m_slots[slot].port         = session_response::get_port(seq);
      m_slots[slot].is_exclusive = true;

      if (m_token.empty()) {
        send_event_to(slot, encode<request_token>());
      } else {
        send_event_to(slot, encode<session_token>(m_token));
      }

      return true;
    }

    if (session_token::scan(seq)) {
      if (slot < 0 || slot >= m_slots.size()) {
        error_("Invalid slot.", __FUNCTION__);
        return true;
      }

      const auto token = session_token::get_token(seq);

      m_token.assign(token.begin(), token.end());

      return true;
    }

    if (server_idle::scan(seq)) {
      if (slot < 0 || slot >= m_slots.size()) {
        error_("Invalid slot.", __FUNCTION__);
        return true;
      }

      const auto index = server_idle::get_idle_index(seq);

      if (index >= 0) {
        const auto &qu          = m_slots[slot].queue;
        const auto  event_count = qu.index + qu.events.size();

        if (index > event_count) {
          auto events = sl::vector<sl::index> {};

          for (sl::index n = event_count; n < index; n++) {
            events.emplace_back(n);

            if (events.size() >= request_events::max_event_count) {
              break;
            }
          }

          send_event_to(slot, encode<request_events>(events));
        }

        update_time_limit(server_idle::get_idle_time(seq));
      }

      return true;
    }

    if (server_quit::scan(seq)) {
      if (slot < 0 || slot >= m_slots.size()) {
        error_("Invalid slot.", __FUNCTION__);
        return true;
      }

      set_connected(false);
      set_quit(true);
      m_node.reset();

      return true;
    }

    return udp_server::perform_control(slot, seq);
  }
}
