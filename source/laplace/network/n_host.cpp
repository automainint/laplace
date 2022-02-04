/*  laplace/network/n_host.cpp
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

#include "host.h"

#include "../engine/protocol/all.h"
#include "crypto/ecc_rabbit.h"

namespace laplace::network {
  namespace access = engine::access;
  using namespace engine::protocol;

  using std::make_unique, engine::encode, engine::id_undefined,
      engine::solver, engine::prime_impact, crypto::ecc_rabbit;

  host::host() {
    auto dev = std::random_device {};
    m_rand.seed(dev());

    setup_world();

    set_max_slot_count(slot_count_unlimited);
    set_master(true);
  }

  void host::set_seed(engine::seed_type seed) {
    if (is_connected()) {
      error_("Unable to set seed.", __FUNCTION__);
      return;
    }

    m_seed = seed;
  }

  void host::listen(uint16_t port) {
    cleanup();
    set_connected(true);

    m_node = m_socket_interface->open(port);

    process_event(slot_host,
                  encode<server_clock>(get_tick_duration()));
    process_event(slot_host, encode<server_seed>(m_seed));
    process_event(slot_host, encode<server_init>());

    perform_instant_events();
  }

  auto host::perform_control(sl::index slot, span_cbyte seq) -> bool {

    if (session_request::scan(seq)) {
      if (slot < 0 || slot >= m_slots.size()) {
        error_("Invalid slot.", __FUNCTION__);
        return true;
      }

      const auto cipher_id = session_request::get_cipher(seq);

      m_slots[slot].token = generate_token();
      m_slots[slot].node  = m_socket_interface->open(any_port);

      if (cipher_id == ids::cipher_ecc_rabbit) {
        m_slots[slot].tran.setup_cipher<ecc_rabbit>();
        m_slots[slot].tran.set_remote_key(
            session_request::get_key(seq));

        send_event_to(slot, encode<session_response>(
                                m_slots[slot].node->get_port(),
                                m_slots[slot].tran.get_public_key()));
      } else {
        if (cipher_id != ids::cipher_plain) {
          if (is_verbose()) {
            verb(fmt("Network: Unknown cipher on slot %d.",
                     (int) slot));
          }
        }

        send_event_to(
            slot, encode<session_response>(
                      m_slots[slot].node->get_port(), span_cbyte {}));
      }

      return true;
    }

    if (session_token::scan(seq)) {
      if (slot < 0 || slot >= m_slots.size()) {
        error_("Invalid slot.", __FUNCTION__);
        return true;
      }

      const auto token = session_token::get_token(seq);

      if (token.empty()) {
        return true;
      }

      for (sl::index i = 0; i < m_slots.size(); i++) {
        auto &s = m_slots[i];

        if (i == slot || s.token.empty() ||
            s.token.size() != token.size()) {
          continue;
        }

        bool equals = true;

        for (sl::index k = 0; k < token.size(); k++)
          if (s.token[k] != token[k]) {
            equals = false;
            break;
          }

        if (equals) {
          if (m_slots[slot].id_actor != id_undefined) {
            process_event(slot, encode<slot_remove>());
          }

          m_slots[slot].token    = s.token;
          m_slots[slot].id_actor = s.id_actor;

          s.id_actor     = id_undefined;
          s.is_connected = false;

          break;
        }
      }

      return true;
    }

    if (client_enter::scan(seq)) {
      if (slot < 0 || slot >= m_slots.size()) {
        error_("Invalid slot.", __FUNCTION__);
        return true;
      }

      if (auto wor = get_world(); wor) {
        if (m_slots[slot].id_actor == id_undefined) {
          m_slots[slot].id_actor = get_world()->reserve(id_undefined);
          process_event(slot, encode<slot_create>(false));
        }
      } else {
        error_("No world.", __FUNCTION__);
      }

      return true;
    }

    if (client_leave::scan(seq)) {
      if (slot < 0 || slot >= m_slots.size()) {
        error_("Invalid slot.", __FUNCTION__);
        return true;
      }

      if (get_state() == server_state::prepare) {
        process_event(slot, encode<slot_remove>());
        m_slots[slot].id_actor = id_undefined;
      }

      m_slots[slot].is_connected = false;
      return true;
    }

    if (client_ready::scan(seq)) {
      if (slot < 0 || slot >= m_slots.size()) {
        error_("Invalid slot.", __FUNCTION__);
        return true;
      }

      if (get_state() == server_state::prepare) {
        add_instant_event(seq);
      } else {
        if (is_verbose()) {
          verb("Network: Ignore ready command.");
        }
      }

      return true;
    }

    return udp_server::perform_control(slot, seq);
  }

  auto host::generate_token() -> vbyte {
    auto v = vbyte(16);
    auto d = std::uniform_int_distribution<int>(0, 255);

    for (sl::index i = 0; i < v.size(); i++) {
      v[i] = static_cast<uint8_t>(d(m_rand));
    }

    return v;
  }
}
