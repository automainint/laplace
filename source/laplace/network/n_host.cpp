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

#include "../engine/protocol/basic_event.h"
#include "../engine/protocol/ids.h"
#include "../engine/protocol/ping.h"
#include "../engine/protocol/public_key.h"
#include "../engine/protocol/request_events.h"
#include "../engine/protocol/server_clock.h"
#include "../engine/protocol/server_seed.h"
#include "../engine/protocol/slot_create.h"
#include "../engine/protocol/slot_remove.h"
#include "host.h"

namespace laplace::network {
  namespace access = engine::access;
  using namespace engine::protocol;
  using std::make_unique, std::string_view, engine::encode,
      engine::prime_impact, engine::time_undefined,
      engine::id_undefined, engine::solver;

  host::host() {
    setup_world();

    set_max_slot_count(slot_count_unlimited);
    set_distribution_enabled(true);
  }

  host::~host() {
    if (m_node) {
      process_event(slot_host, encode<server_quit>());

      send_events();
    }
  }

  void host::listen(uint16_t port) {
    set_connected(true);

    m_node = make_unique<udp_node>();
    m_node->bind(port);

    process_event( //
        slot_host, //
        encode<server_clock>(get_tick_duration()));

    process_event( //
        slot_host, //
        encode<server_seed>(solver::generate_seed()));

    process_event( //
        slot_host, //
        encode<server_init>());
  }

  void host::queue(cref_vbyte seq) {
    if (!seq.empty()) {
      process_event(slot_host, seq);
    } else {
      verb("[ host ] ignore empty event");
    }
  }

  auto host::perform_control(size_t slot, cref_vbyte seq) -> bool {

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

    if (server_init::scan(seq)) {
      /*  Send to the clients.
       */
      return false;
    }

    if (server_launch::scan(seq)) {
      /*  Send to the clients.
       */
      return false;
    }

    if (server_action::scan(seq)) {
      set_state(server_state::action);

      /*  Send to the clients.
       */
      return false;
    }

    if (server_pause::scan(seq)) {
      set_state(server_state::pause);

      /*  Send to the clients.
       */
      return false;
    }

    if (server_clock::scan(seq)) {
      set_tick_duration(server_clock::get_tick_duration(seq));
      return false;
    }

    if (server_seed::scan(seq)) {
      if (get_state() == server_state::prepare) {
        set_random_seed(server_seed::get_seed(seq));
        return false;

      } else {
        /*  It makes no sense to change the seed
         *  after preparation.
         */
        verb("[ host ] ignore seed command");
      }

      return true;
    }

    if (server_quit::scan(seq)) {
      /*  Send to the clients.
       */
      return false;
    }

    if (public_key::scan(seq)) {
      if (public_key::get_cipher(seq) == ids::cipher_dh_rabbit) {

        send_event_to(                 //
            slot,                      //
            encode<public_key>(        //
                ids::cipher_dh_rabbit, //
                m_slots[slot].cipher.get_public_key()));

        m_slots[slot].is_encrypted = false;
        m_slots[slot].cipher.set_remote_key(public_key::get_key(seq));

        if (is_verbose()) {
          verb("[ host ] mutual key");
        }

        dump(m_slots[slot].cipher.get_mutual_key());
      } else {
        verb("[ host ] unknown cipher");
      }

      return true;
    }

    if (request_events::scan(seq) && slot != slot_host) {
      const auto count = request_events::get_event_count(seq);

      for (size_t i = 0; i < count; i++) {
        auto n = request_events::get_event(seq, i);

        if (n < m_queue.events.size()) {
          send_event_to(slot, m_queue.events[n]);
        } else if (is_verbose()) {
          verb("[ host ] no requested event %zu", n);
        }
      }

      return true;
    }

    if (client_enter::scan(seq) && slot != slot_host) {
      if (auto wor = get_world(); wor) {
        m_slots[slot].id_actor = get_world()->reserve(id_undefined);

        process_event(slot, encode<slot_create>(false));
      } else {
        error(__FUNCTION__, "No world.");
      }

      return true;
    }

    if (client_leave::scan(seq) && slot != slot_host) {
      if (get_state() == server_state::prepare) {
        process_event(slot, encode<slot_remove>());

        m_slots[slot].id_actor = id_undefined;
      }

      m_slots[slot].is_connected = false;
      return true;
    }

    if (client_ready::scan(seq) && slot != slot_host) {
      if (get_state() == server_state::prepare) {
        if (auto f = get_factory(); f) {
          auto ev = f->decode(seq);

          if (ev) {
            if (auto wor = get_world(); wor) {
              ev->perform({ *get_world(), access::sync });
            } else {
              error(__FUNCTION__, "No world.");
            }
          } else {
            verb("[ host ] unable to decode command");
          }
        } else {
          error(__FUNCTION__, "No factory.");
        }
      } else {
        verb("[ host ] ignore ready command");
      }

      return true;
    }

    return false;
  }
}
