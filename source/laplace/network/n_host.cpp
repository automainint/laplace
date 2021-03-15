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
  namespace pro    = engine::protocol;

  using std::make_unique, engine::encode, engine::id_undefined,
      engine::solver, engine::prime_impact, pro::client_enter,
      pro::client_leave, pro::client_ready, pro::server_clock,
      pro::server_seed, pro::server_init, pro::slot_create,
      pro::slot_remove;

  host::host() {
    setup_world();

    set_max_slot_count(slot_count_unlimited);
    set_master(true);
  }

  void host::set_seed(engine::seed_type seed) {
    if (is_connected()) {
      error(__FUNCTION__, "Unable to set seed.");
      return;
    }

    m_seed = seed;
  }

  void host::listen(uint16_t port) {
    cleanup();
    set_connected(true);

    m_node = make_unique<udp_node>();
    m_node->bind(port);

    process_event(slot_host, encode<server_clock>(get_tick_duration()));
    process_event(slot_host, encode<server_seed>(m_seed));
    process_event(slot_host, encode<server_init>());

    perform_instant_events();
  }

  auto host::perform_control(size_t slot, cref_vbyte seq) -> bool {

    if (client_enter::scan(seq) && slot != slot_host) {
      if (auto wor = get_world(); wor) {
        if (m_slots[slot].id_actor == id_undefined) {
          m_slots[slot].id_actor = get_world()->reserve(id_undefined);
          process_event(slot, encode<slot_create>(false));
        }
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
            add_instant_event(prime_impact::get_id(seq), ev);
          } else {
            verb("Network: Unable to decode command.");
          }
        } else {
          error(__FUNCTION__, "No factory.");
        }
      } else {
        verb("Network: Ignore ready command.");
      }

      return true;
    }

    return udp_server::perform_control(slot, seq);
  }
}
