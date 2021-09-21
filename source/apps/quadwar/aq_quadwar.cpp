/*  apps/quadwar/aq_quadwar.cpp
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

#include "quadwar.h"

#include "../../laplace/core/utils.h"
#include "../../laplace/graphics/utils.h"
#include "../../laplace/stem/config.h"
#include <fstream>
#include <random>

namespace quadwar_app {
  using std::make_shared, core::family, std::string, std::u8string_view;

  const char8_t *quadwar::default_player_names[] = {
    u8"Alice",     u8"Bob",      u8"Foo",      u8"Bar",
    u8"Newbye",    u8"Gosu",     u8"Decartes", u8"Socrates",
    u8"Spinosa",   u8"Pluto",    u8"Monke",    u8"Deleuze",
    u8"Sorceress", u8"Daemon",   u8"Warlock",  u8"Ranger",
    u8"Priest",    u8"Doge",     u8"Glider",   u8"Skeleton",
    u8"Peon",      u8"Baron",    u8"Warlord",  u8"Count",
    u8"Point",     u8"Triangle", u8"Crown",    u8"Blade"
  };

  const char *    quadwar::default_server_address = "127.0.0.1";
  const char *    quadwar::default_game_name      = "Unknown";
  const sl::whole quadwar::default_player_count   = 4u;
  const sl::whole quadwar::default_unit_count     = 6u;
  const sl::whole quadwar::default_map_size       = 64u;

  const char8_t *quadwar::caption = u8"Quadwar";

  auto quadwar::get_player_name() -> u8string_view {
    constexpr auto count = sizeof default_player_names /
                           sizeof *default_player_names;

    static_assert(count > 0);

    auto dev  = std::random_device {};
    auto dist = std::uniform_int_distribution<sl::index>(0, count - 1);

    return default_player_names[dist(dev)];
  }

  auto quadwar::get_config() -> family {
    using namespace stem::config;

    auto cfg = get_default();

    cfg[k_caption]        = caption;
    cfg[k_server_address] = default_server_address;
    cfg[k_game_name]      = default_game_name;
    cfg[k_player_name]    = get_player_name();
    cfg[k_map_size]       = default_map_size;
    cfg[k_player_count]   = default_player_count;
    cfg[k_unit_count]     = default_unit_count;

    return cfg;
  }

  quadwar::quadwar(int argc, char **argv) :
      app_flat(argc, argv, get_config()) { }

  void quadwar::init() {
    app_flat::init();

    m_mainmenu = make_shared<ui::mainmenu>();
    m_mainmenu->attach_to(m_ui);

    m_mainmenu->set_server_address(
        m_config[k_server_address].get_string());
    m_mainmenu->set_game_name(m_config[k_game_name].get_string());
    m_mainmenu->set_player_name(m_config[k_player_name].get_string());
    m_mainmenu->set_map_size(m_config[k_map_size].get_uint());
    m_mainmenu->set_player_count(m_config[k_player_count].get_uint());
    m_mainmenu->set_unit_count(m_config[k_unit_count].get_uint());

    auto return_to_mainmenu = [this] {
      m_return_to_mainmenu = true;

      m_mainmenu->refresh();
      m_mainmenu->set_visible(true);
    };

    auto quit = [this] {
      get_window().quit();
    };

    auto init_session = [this, return_to_mainmenu, quit] {
      m_mainmenu->set_visible(false);

      m_session = make_shared<session>();
      m_session->attach_to(m_ui);

      m_session->adjust_layout(m_ui->get_rect().width,
                               m_ui->get_rect().height);

      m_session->on_done(return_to_mainmenu);
      m_session->on_quit(quit);
    };

    m_mainmenu->on_create(
        [this, init_session](ui::mainmenu::create_info info) {
          init_session();

          m_session->set_game_name(info.game_name);
          m_session->set_player_name(info.player_name);
          m_session->set_map_size(info.map_size);
          m_session->set_player_count(info.player_count);
          m_session->set_unit_count(info.unit_count);

          m_session->create();
        });

    m_mainmenu->on_join(
        [this, init_session](ui::mainmenu::join_info info) {
          init_session();

          m_session->set_server_ip(info.server_ip);
          m_session->set_game_name(info.game_name);
          m_session->set_player_name(info.player_name);

          m_session->join();
        });

    m_mainmenu->on_quit(quit);
  }

  void quadwar::cleanup() {
    m_mainmenu.reset();
    m_session.reset();

    app_flat::cleanup();
  }

  void quadwar::update(sl::time delta_msec) {
    app_flat::update(delta_msec);

    if (m_session) {
      m_session->tick(delta_msec, get_input());

      if (m_return_to_mainmenu) {
        m_session.reset();
        m_return_to_mainmenu = false;
      }
    }
  }

  void quadwar::render() {
    graphics::clear(clear_color);

    if (m_session) {
      m_session->render();
    }

    m_ui->render();

    get_gl().swap_buffers();
  }

  void quadwar::adjust_layout(sl::whole width, sl::whole height) {
    app_flat::adjust_layout(width, height);

    m_mainmenu->adjust_layout(width, height);

    if (m_session) {
      m_session->adjust_layout(width, height);
    }
  }
}
