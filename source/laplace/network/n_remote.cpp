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

#include "../engine/protocol/basic_event.h"
#include "../engine/protocol/ping.h"
#include "../engine/protocol/public_key.h"
#include "../engine/protocol/request_events.h"
#include "../engine/protocol/server_clock.h"
#include "../engine/protocol/server_idle.h"
#include "../engine/protocol/server_seed.h"
#include "remote.h"

namespace laplace::network {
  namespace access = engine::access;
  using namespace engine::protocol;
  using std::min, std::make_unique, std::string, std::string_view,
      engine::encode, engine::prime_impact, engine::time_undefined;

  remote::remote() {
    setup_world();

    set_performing_enabled(true);
  }

  remote::~remote() {
    cleanup();
  }

  void remote::connect(string_view host_address, uint16_t host_port,
                       uint16_t client_port) {
    cleanup();

    set_connected(true);

    m_node = make_unique<udp_node>();
    m_node->bind(client_port);

    add_slot(host_address, host_port);

    send_event(encode<public_key>( //
        ids::cipher_dh_rabbit,     //
        m_slots[0].cipher.get_public_key()));

    emit<client_enter>();
  }

  void remote::queue(cref_vbyte seq) {
    if (!seq.empty()) {
      auto &qu = m_queue.events;

      qu.emplace_back(vbyte(seq.begin(), seq.end()));

      prime_impact::set_index(qu.back(), qu.size() - 1);
    }
  }

  void remote::cleanup() {
    if (m_node) {
      queue(encode<client_leave>());

      send_events();
    }

    m_queue.index = 0;
    m_queue.events.clear();
    m_slots.clear();

    set_connected(false);
  }

  auto remote::perform_control(size_t slot, cref_vbyte seq) -> bool {

    if (public_key::scan(seq)) {
      if (public_key::get_cipher(seq) == ids::cipher_dh_rabbit) {
        if (slot < m_slots.size()) {
          m_slots[slot].is_encrypted = true;

          m_slots[slot].cipher.set_remote_key(public_key::get_key(seq));

          if (is_verbose()) {
            verb("[ remote ] mutual key");
          }

          dump(m_slots[slot].cipher.get_mutual_key());
        } else {
          error(__FUNCTION__, "Invalid slot.");
        }
      }

      return true;
    }

    if (request_events::scan(seq)) {
      const auto count = request_events::get_event_count(seq);

      for (size_t i = 0; i < count; i++) {
        auto n = request_events::get_event(seq, i);

        if (n < m_queue.events.size()) {
          send_event(m_queue.events[n]);
        }
      }

      return true;
    }

    if (server_idle::scan(seq)) {
      const auto time = server_idle::get_idle_time(seq);

      update_time_limit(time);
      return true;
    }

    if (server_launch::scan(seq)) {
      /*  Perform event.
       */
      return false;
    }

    if (server_action::scan(seq)) {
      set_state(server_state::action);
      return true;
    }

    if (server_pause::scan(seq)) {
      set_state(server_state::pause);
      return true;
    }

    if (server_clock::scan(seq)) {
      set_tick_duration(server_clock::get_tick_duration(seq));
      return true;
    }

    if (server_seed::scan(seq)) {
      const auto seed = server_seed::get_seed(seq);

      if (get_state() == server_state::prepare) {
        set_random_seed(seed);
      }

      return true;
    }

    if (server_quit::scan(seq)) {
      m_node.reset();
      return true;
    }

    if (ping_request::scan(seq)) {
      const auto time = ping_request::get_ping_time(seq);
      send_event_to(slot, encode<ping_response>(time));
      return true;
    }

    if (ping_response::scan(seq)) {
      const auto ping = get_local_time() -
                        ping_response::get_ping_time(seq);

      set_ping(ping);
      return true;
    }

    return false;
  }
}
