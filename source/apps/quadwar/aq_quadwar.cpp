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

#include "../../laplace/stem/config.h"
#include "quadwar.h"

using namespace quadwar_app;
using namespace core;
using namespace std;

auto quadwar::get_config() -> family {
  using namespace stem::config;
  family cfg     = get_default();
  cfg[k_caption] = caption;
  return cfg;
}

quadwar::quadwar(int argc, char **argv) :
    app_flat(argc, argv, get_config()) { }

void quadwar::init() {
  app_flat::init();

  m_mainmenu = make_shared<mainmenu>();
  m_mainmenu->attach_to(m_ui);

  auto return_to_mainmenu = [=] {
    m_session.reset();

    m_mainmenu->refresh();
    m_mainmenu->set_visible(true);
  };

  auto quit = [=] {
    get_window().quit();
  };

  auto init_session = [=] {
    m_mainmenu->set_visible(false);

    m_session = make_shared<session>();
    m_session->attach_to(m_ui);

    m_session->adjust_layout(get_window().get_frame_width(),
                             get_window().get_frame_height());

    m_session->on_done(return_to_mainmenu);
    m_session->on_quit(quit);
  };

  m_mainmenu->on_create([=](mainmenu::create_info info) {
    init_session();

    m_session->set_game_name(info.game_name);
    m_session->set_player_name(info.player_name);
    m_session->set_map_size(info.map_size);
    m_session->set_player_count(info.player_count);
    m_session->set_unit_count(info.unit_count);

    m_session->create();
  });

  m_mainmenu->on_join([=](mainmenu::join_info info) {
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

void quadwar::update(size_t delta_msec) {
  app_flat::update(delta_msec);

  if (m_session) {
    m_session->tick(delta_msec);
  }
}

void quadwar::render() {
  app_flat::render();
}

void quadwar::set_frame_size(size_t width, size_t height) {
  m_mainmenu->adjust_layout(width, height);

  if (m_session) {
    m_session->adjust_layout(width, height);
  }

  app_flat::set_frame_size(width, height);
}
