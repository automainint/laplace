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
#include "../../laplace/engine/protocol/basic_event.h"
#include "../../laplace/engine/protocol/slot_create.h"
#include "../../laplace/network/host.h"
#include "../../laplace/network/remote.h"
#include "../../laplace/platform/keys.h"
#include "object/player.h"
#include "object/root.h"
#include "protocol/qw_loading.h"
#include "protocol/qw_player_name.h"
#include "session.h"
#include <filesystem>
#include <fstream>
#include <thread>

namespace quadwar_app {
  namespace access      = engine::access;
  namespace this_thread = std::this_thread;

  using std::find, std::make_shared, std::string, std::string_view,
      std::u8string_view, std::ofstream, std::ifstream, network::host,
      network::remote, protocol::qw_player_name, object::root,
      object::player, engine::id_undefined, platform::ref_input,
      view::vec2, view::real;

  session::session() {
    m_lobby.on_abort([this] {
      if (m_on_done)
        m_on_done();
    });

    m_lobby.on_start([this] {
      if (m_server) {

        m_server->emit<protocol::qw_loading>( //
            m_map_size,                       //
            m_player_count,                   //
            m_unit_count);

        m_server->emit<protocol::server_launch>();
        m_server->emit<protocol::server_action>();

        m_lobby.on_start([] {});
      }
    });
  }

  session::~session() {
    if (m_host_info_saved) {
      std::filesystem::remove(host_info_file);
    }
  }

  void session::on_done(session::event_done ev) {
    m_on_done = ev;
  }

  void session::on_quit(session::event_quit ev) {
    m_on_quit = ev;
  }

  void session::tick(uint64_t delta_msec, ref_input in) {
    if (m_server && m_world) {
      m_server->tick(delta_msec);

      if (m_server->is_connected()) {
        update_control(delta_msec, in);

        update_lobby();

      } else if (m_server->is_quit()) {
        if (m_on_done) {
          m_on_done();
        }
      } else {
        this_thread::yield();

        verb("Session: Trying to reconnect...");

        m_server->reconnect();

        m_server->emit<qw_player_name>(m_player_name);
      }
    }
  }

  void session::render() {
    if (m_world) {
      m_view.render(*m_world);
    }
  }

  void session::attach_to(ui::ptr_widget w) {
    m_lobby.attach_to(w);
  }

  void session::adjust_layout(int width, int height) {
    m_lobby.adjust_layout(width, height);
    m_view.adjust_layout(width, height);
  }

  void session::set_server_ip(string_view server_ip) {
    auto i = find(server_ip.begin(), server_ip.end(), ':');

    if (i == server_ip.end()) {
      m_server_ip = server_ip;
    } else {
      const auto p    = &(i + 1)[0];
      const auto size = static_cast<size_t>(server_ip.end() - (i + 1));

      m_server_ip = string(server_ip.begin(), i);

      m_server_port = static_cast<uint16_t>(to_integer({ p, size }));
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

    m_server = server;
    m_world  = server->get_world();

    server->set_verbose(true);
    server->set_allowed_commands(allowed_commands);
    server->make_factory<qw_factory>();

    server->listen(m_server_port);

    /*  Create the host actor.
     */
    const auto id_actor = m_world->reserve(id_undefined);

    server->emit<protocol::slot_create>(id_actor);
    server->emit<qw_player_name>(id_actor, m_player_name);

    save_host_info(server->get_port());

    m_lobby.show_info(fmt(u8"Game host (port %hu)", server->get_port()));

    m_lobby.set_start_enabled(true);
  }

  void session::join() {
    auto server = make_shared<remote>();

    m_server = server;
    m_world  = server->get_world();

    server->set_verbose(true);
    server->make_factory<qw_factory>();

    server->connect(m_server_ip, m_server_port);

    server->emit<qw_player_name>(m_player_name);

    m_lobby.show_info(u8"Game guest");
    m_lobby.set_start_enabled(false);
  }

  auto session::get_host_address(string_view default_address)
      -> string {

    auto f = ifstream(session::host_info_file);

    if (f) {
      auto port = network::any_port;
      f >> port;

      if (f) {
        verb(fmt("Host address found: %s:%hu", network::localhost, port));
        return fmt("%s:%hu", network::localhost, port);
      }
    } else if (auto f2 = ifstream(session::host_info_file_debug); f2) {
      auto port = network::any_port;
      f2 >> port;

      if (f2) {
        verb(fmt("Host address found: %s:%hu", network::localhost, port));
        return fmt("%s:%hu", network::localhost, port);
      }
    }

    return string(default_address);
  }

  void session::update_control(uint64_t delta_msec, ref_input in) {
    bool is_moved = false;
    vec2 delta;

    const auto fdelta = static_cast<real>(delta_msec);

    if (in.is_key_down(platform::key_left)) {
      is_moved = true;
      delta.x() -= sense_move * fdelta;
    }

    if (in.is_key_down(platform::key_right)) {
      is_moved = true;
      delta.x() += sense_move * fdelta;
    }

    if (in.is_key_down(platform::key_up)) {
      is_moved = true;
      delta.y() -= sense_move * fdelta;
    }

    if (in.is_key_down(platform::key_down)) {
      is_moved = true;
      delta.y() += sense_move * fdelta;
    }

    if (in.get_wheel_delta() != 0) {
      const auto fwheel = static_cast<real>(in.get_wheel_delta());
      m_view.scale(sense_scale * fwheel);
    }

    if (in.is_key_down(platform::key_mbutton)) {
      is_moved = true;
      delta.x() -= static_cast<real>(in.get_mouse_delta_x());
      delta.y() -= static_cast<real>(in.get_mouse_delta_y());
    }

    if (is_moved) {
      m_view.move(delta);
    }
  }

  void session::update_lobby() {
    if (!m_lobby.is_visible())
      return;

    auto r = m_world->get_entity(m_world->get_root());

    const auto ver = root::get_version({ r, access::read_only });

    if (m_root_ver < ver) {
      m_root_ver = ver;

      if (root::is_launched({ r, access::read_only })) {
        m_lobby.set_visible(false);
        return;
      }

      if (root::is_loading({ r, access::read_only })) {
        m_lobby.show_loading();
        return;
      }

      const auto count = root::get_slot_count({ r, access::read_only });

      for (size_t i = 0; i < count; i++) {

        const auto id_actor = root::get_slot(
            { r, access::read_only }, i);
        const auto actor    = m_world->get_entity(id_actor);
        const auto is_local = player::is_local(
            { actor, access::read_only });

        if (is_local)
          m_id_actor = id_actor;

        auto name = player::get_name({ actor, access::read_only });

        if (name.empty()) {
          name = u8"[ Reserved ]";
        }

        if (is_local) {
          name.append(u8" *");
        }

        m_lobby.set_slot(i, id_actor, name);
      }

      for (size_t i = count; i < m_lobby.get_slot_count(); i++) {
        m_lobby.remove_slot(i);
      }
    }
  }

  void session::save_host_info(uint16_t port) {
    auto f = ofstream(host_info_file);

    if (f) {
      f << port;
      m_host_info_saved = true;
    }
  }
}
