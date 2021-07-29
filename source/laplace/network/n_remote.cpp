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

#include "../engine/protocol/basic_event.h"
#include "../engine/protocol/ping.h"
#include "../engine/protocol/public_key.h"
#include "../engine/protocol/request_events.h"
#include "../engine/protocol/server_clock.h"
#include "../engine/protocol/server_idle.h"
#include "../engine/protocol/server_seed.h"
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

    m_node = make_unique<udp_node>();
    m_node->bind(m_client_port);

    auto slot = add_slot(m_host_address, m_host_port);

    if (is_encryption_enabled()) {
      m_slots[slot].tran.setup_cipher<ecc_rabbit>();
      const auto key = m_slots[slot].tran.get_public_key();
      send_event(encode<public_key>(ids::cipher_ecc_rabbit, key));
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

    if (server_idle::scan(seq)) {
      const auto index = server_idle::get_idle_index(seq);

      if (index != -1) {
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
      set_connected(false);
      set_quit(true);
      m_node.reset();
      return true;
    }

    return udp_server::perform_control(slot, seq);
  }
}
