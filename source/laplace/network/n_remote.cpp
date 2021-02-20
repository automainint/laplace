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

    set_master(false);
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

  auto remote::perform_control(size_t slot, cref_vbyte seq) -> bool {

    if (server_idle::scan(seq)) {
      update_time_limit(server_idle::get_idle_time(seq));
      return true;
    }

    if (server_quit::scan(seq)) {
      set_connected(false);
      m_node.reset();
      return true;
    }

    return udp_server::perform_control(slot, seq);
  }
}
