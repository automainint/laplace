/*  apps/quadwar/aq_session.cpp
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

#include "../../laplace/core/utils.h"
#include "../../laplace/engine/host.h"
#include "../../laplace/engine/remote.h"
#include "player.h"
#include "qw_player_name.h"
#include "session.h"

namespace quadwar_app {
  using std::find, std::make_shared, std::string,
      std::string_view, std::u8string_view;

  session::session() {
    m_lobby.on_abort([=] {
      if (m_on_done)
        m_on_done();
    });
  }

  void session::on_done(session::event_done ev) {
    m_on_done = ev;
  }

  void session::on_quit(session::event_quit ev) {
    m_on_quit = ev;
  }

  void session::tick(size_t delta_msec) {
    if (m_server && m_root && m_world) {
      m_server->tick(delta_msec);

      if (m_server->is_connected()) {
        if (m_root->changed()) {
          auto count = m_root->get_slot_count();

          for (size_t i = 0; i < count; i++) {
            const auto id_actor = m_root->get_slot(i);
            const auto actor    = m_world->get_entity(id_actor);

            const auto is_local = player::is_local(
                { actor, access::async });

            if (is_local)
              m_id_actor = id_actor;

            auto name = player::get_name({ actor, access::async });

            if (name.empty()) {
              name = u8"[ Reserved ]";
            }

            if (is_local) {
              name.append(u8" *");
            }

            m_lobby.set_slot(i, id_actor, name);
          }

          for (size_t i = count; i < m_lobby.get_slot_count();
               i++) {
            m_lobby.remove_slot(i);
          }
        }
      } else {
        if (m_on_done)
          m_on_done();
      }
    }
  }

  void session::attach_to(ui::ptr_widget w) {
    m_lobby.attach_to(w);
  }

  void session::adjust_layout(int width, int height) {
    m_lobby.adjust_layout(width, height);
  }

  void session::set_server_ip(string_view server_ip) {
    auto i = find(server_ip.begin(), server_ip.end(), ':');

    if (i == server_ip.end()) {
      m_server_ip = server_ip;
    } else {
      const auto   p    = &(i + 1)[0];
      const size_t size = server_ip.end() - (i + 1);

      m_server_ip = string(server_ip.begin(), i);

      m_server_port = static_cast<uint16_t>(
          to_integer({ p, size }));
    }
  }

  void session::set_server_port(uint16_t port) {
    m_server_port = port;
  }

  void session::set_game_name(u8string_view game_name) {
    m_game_name = game_name;
  }

  void session::set_player_name(u8string_view player_name) {
    m_player_name = player_name;
  }

  void session::set_map_size(size_t map_size) {
    m_map_size = map_size;
  }

  void session::set_player_count(size_t player_count) {
    m_player_count = player_count;
  }

  void session::set_unit_count(size_t unit_count) {
    m_unit_count = unit_count;
  }

  void session::create() {
    auto server = make_shared<host>();

    m_world = server->get_world();
    m_root  = make_shared<root>();

    m_world->set_root(m_root);

    server->set_allowed_commands(allowed_commands);
    server->set_factory(m_factory);

    server->listen(m_server_port);

    /*  Create the host actor.
     */
    auto id_actor = m_world->reserve(id_undefined);

    server->queue<protocol::slot_create>(id_actor);

    server->queue<qw_player_name>(id_actor, m_player_name);

    m_lobby.show_info(to_u8string(
        u8"Game host (port %hu)", server->get_port()));

    m_lobby.set_start_enabled(true);

    m_server = server;
  }

  void session::join() {
    auto server = make_shared<remote>();

    m_world = server->get_world();
    m_root  = make_shared<root>();

    m_world->set_root(m_root);

    server->set_factory(m_factory);

    server->connect(m_server_ip, m_server_port);

    server->queue<qw_player_name>(m_player_name);

    m_lobby.show_info(u8"Game guest");
    m_lobby.set_start_enabled(false);

    m_server = server;
  }
}
