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

#include "session.h"

#include "../../laplace/core/keys.h"
#include "../../laplace/core/utils.h"
#include "../../laplace/engine/protocol/basic_event.h"
#include "../../laplace/engine/protocol/slot_create.h"
#include "../../laplace/network/host.h"
#include "../../laplace/network/remote.h"
#include "object/landscape.h"
#include "object/player.h"
#include "object/root.h"
#include "protocol/qw_loading.h"
#include "protocol/qw_order_move.h"
#include "protocol/qw_player_name.h"
#include <filesystem>
#include <fstream>
#include <limits>
#include <thread>

namespace quadwar_app {
  namespace access      = engine::access;
  namespace this_thread = std::this_thread;
  namespace ids         = protocol::ids;

  using namespace core::keys;

  using std::find, std::make_shared, std::string, std::string_view,
      std::u8string_view, std::ofstream, std::ifstream, network::host,
      network::remote, protocol::qw_player_name, object::root,
      object::landscape, object::player, engine::id_undefined,
      core::cref_input_handler, view::vec2, view::real,
      core::is_key_down, core::get_wheel_delta;

  const uint16_t session::allowed_commands[] = {
    ids::session_request, ids::session_token, ids::request_token,
    ids::request_events,  ids::ping_request,  ids::ping_response,
    ids::client_enter,    ids::client_leave,  ids::client_ready,
    ids::player_name,     ids::order_move
  };

  char const     session::host_info_file[]    = ".host";
  char const     session::default_server_ip[] = "127.0.0.1";
  uint16_t const session::default_port        = network::any_port;

  sl::whole const session::thread_count = 4;
  float const     session::sense_move   = 1.5f;
  float const     session::sense_scale  = .0003f;

  session::session() {
    m_lobby.on_abort([this] {
      if (m_on_done)
        m_on_done();
    });

    m_lobby.on_start([this] {
      if (m_server) {

        m_server->emit<protocol::qw_loading>(
            m_map_size, m_player_count, m_unit_count);

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

  void session::tick(sl::time delta_msec, cref_input_handler in) {
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
        verb("Session: Trying to reconnect...");

        m_server->reconnect();

        m_server->emit<qw_player_name>(m_player_name);
      }
    }
  }

  void session::render() {
    if (m_world && m_show_game) {
      m_view.render(*m_world);
    }
  }

  void session::attach_to(ui::ptr_widget w) {
    m_lobby.attach_to(std::move(w));
  }

  void session::adjust_layout(sl::whole width, sl::whole height) {
    m_lobby.adjust_layout(width, height);
    m_view.adjust_layout(width, height);
  }

  void session::set_server_ip(string_view server_ip) {
    auto const i = find(server_ip.begin(), server_ip.end(), ':');

    if (i == server_ip.end()) {
      m_server_ip = server_ip;
    } else {
      auto const as_port = [](auto const n) -> uint16_t {
        if (n < 0 || n > std::numeric_limits<uint16_t>::max())
          return 0;
        return static_cast<uint16_t>(n);
      };

      m_server_ip   = string { server_ip.begin(), i };
      m_server_port = as_port(to_integer({ i + 1, server_ip.end() }));
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

  void session::set_map_size(sl::whole map_size) {
    m_map_size = map_size;
  }

  void session::set_player_count(sl::whole player_count) {
    m_player_count = player_count;
  }

  void session::set_unit_count(sl::whole unit_count) {
    m_unit_count = unit_count;
  }

  void session::create() {
    auto server = make_shared<host>();

    m_server = server;
    m_world  = server->get_world();

    m_world->set_thread_count(thread_count);

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

    m_lobby.show_info(
        fmt(u8"Game host (port %hu)", server->get_port()));

    m_lobby.set_start_enabled(true);
  }

  void session::join() {
    auto server = make_shared<remote>();

    m_server = server;
    m_world  = server->get_world();

    m_world->set_thread_count(thread_count);

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
        verb(fmt("Host address found: %s:%hu", network::localhost,
                 port));
        return fmt("%s:%hu", network::localhost, port);
      }
    }

    return string(default_address);
  }

  void session::update_control(sl::time           delta_msec,
                               cref_input_handler in) {
    bool is_moved = false;
    auto delta    = vec2 {};

    const auto fdelta = static_cast<real>(delta_msec);

    for (auto const &ev : in.get_events()) {
      if (ev.key == key_wheel) {
        auto const fwheel = static_cast<real>(get_wheel_delta(ev));
        m_view.scale(sense_scale * fwheel);
        continue;
      }

      m_view.set_cursor({ static_cast<real>(ev.cursor_x),
                          static_cast<real>(ev.cursor_y) });

      if (!is_key_down(ev))
        continue;

      switch (ev.key) {
        case key_lbutton: m_view.click(); break;

        case key_rbutton: {
          const auto u = m_view.get_unit();

          if (u >= 0) {
            const auto target = m_view.get_grid_position();

            m_server->emit<protocol::qw_order_move>(
                m_id_actor, u, target.x(), target.y());
          }
        } break;
      }
    }

    m_view.set_cursor({ static_cast<real>(in.get_cursor_x()),
                        static_cast<real>(in.get_cursor_y()) });

    if (in.is_key_down(key_left)) {
      is_moved = true;
      delta.x() -= sense_move * fdelta;
    }

    if (in.is_key_down(key_right)) {
      is_moved = true;
      delta.x() += sense_move * fdelta;
    }

    if (in.is_key_down(key_up)) {
      is_moved = true;
      delta.y() -= sense_move * fdelta;
    }

    if (in.is_key_down(key_down)) {
      is_moved = true;
      delta.y() += sense_move * fdelta;
    }

    if (in.is_key_down(key_mbutton)) {
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

    auto world = access::world({ *m_world, access::read_only });

    auto r     = world.get_entity(world.get_root());
    auto slots = world.get_entity(root::get_slots(r));

    const auto ver        = root::get_version(r);
    const auto slot_count = slots.vec_get_size();

    if (m_root_ver < ver) {
      m_root_ver = ver;

      if (root::is_launched(r)) {
        m_lobby.set_visible(false);
        m_show_game = true;

        auto slot_index = sl::index {};

        for (sl::index i = 0; i < slot_count; i++) {
          if (slots.vec_get(i) == m_id_actor) {
            slot_index = i;
            break;
          }
        }

        auto land      = world.get_entity(root::get_landscape(r));
        auto start_loc = landscape::get_start_loc(land, slot_index);

        const auto x = static_cast<real>(start_loc.x());
        const auto y = static_cast<real>(start_loc.y());

        m_view.set_position({ x, y });

        return;
      }

      if (root::is_loading(r)) {
        m_lobby.show_loading();
        return;
      }

      for (sl::index i = 0; i < slot_count; i++) {

        const auto id_actor = slots.vec_get(i);
        const auto actor    = world.get_entity(id_actor);
        const auto is_local = player::is_local(actor);

        if (is_local) {
          m_id_actor = id_actor;
        }

        auto name = player::get_name(actor);

        if (name.empty()) {
          name = u8"[ Reserved ]";
        }

        if (is_local) {
          name.append(u8" *");
        }

        m_lobby.set_slot(i, id_actor, name);
      }

      for (sl::index i = slot_count; i < m_lobby.get_slot_count();
           i++) {
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
