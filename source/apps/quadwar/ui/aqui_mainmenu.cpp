/*  apps/quadwar/ui/auiq_mainmenu.cpp
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

#include "mainmenu.h"

#include "../../../laplace/core/utils.h"
#include "../session.h"

namespace quadwar_app::ui {
  using ui::column_layout, ui::ptr_widget, ui::rect, std::u8string,
      std::u8string_view, std::tuple;

  sl::whole const mainmenu::spacing     = 4;
  sl::whole const mainmenu::line_count  = 7;
  sl::whole const mainmenu::menu_width  = 640;
  sl::whole const mainmenu::info_height = 80;
  sl::whole const mainmenu::line_height = 60;

  u8string_view const mainmenu::c_welcome      = u8"Welcome";
  u8string_view const mainmenu::c_create       = u8"Create";
  u8string_view const mainmenu::c_join         = u8"Join";
  u8string_view const mainmenu::c_quit         = u8"Quit";
  u8string_view const mainmenu::c_create_game  = u8"Create Game";
  u8string_view const mainmenu::c_join_game    = u8"Join Game";
  u8string_view const mainmenu::c_server_ip    = u8"Server IP";
  u8string_view const mainmenu::c_game_name    = u8"Game name";
  u8string_view const mainmenu::c_player_name  = u8"Player name";
  u8string_view const mainmenu::c_map_size     = u8"Map size";
  u8string_view const mainmenu::c_player_count = u8"Player count";
  u8string_view const mainmenu::c_unit_count   = u8"Unit count";
  u8string_view const mainmenu::c_cancel       = u8"Cancel";
  u8string_view const mainmenu::c_continue     = u8"Continue";

  mainmenu::mainmenu() {
    m_page_root->attach(m_create);
    m_page_root->attach(m_join);
    m_page_root->attach(m_quit);

    m_page_root->set_layout(
        column_layout(menu_width, line_height, spacing));

    m_page_create->attach(m_c_game_name_label);
    m_page_create->attach(m_c_game_name);
    m_page_create->attach(m_c_player_name_label);
    m_page_create->attach(m_c_player_name);
    m_page_create->attach(m_c_map_size_label);
    m_page_create->attach(m_c_map_size);
    m_page_create->attach(m_c_player_count_label);
    m_page_create->attach(m_c_player_count);
    m_page_create->attach(m_c_unit_count_label);
    m_page_create->attach(m_c_unit_count);
    m_page_create->attach(m_c_cancel);
    m_page_create->attach(m_c_continue);

    m_page_create->set_layout(
        column_layout(tuple { menu_width / 2, menu_width / 2 },
                      line_height, spacing));

    m_page_join->attach(m_j_server_ip_label);
    m_page_join->attach(m_j_server_ip);
    m_page_join->attach(m_j_game_name_label);
    m_page_join->attach(m_j_game_name);
    m_page_join->attach(m_j_player_name_label);
    m_page_join->attach(m_j_player_name);
    m_page_join->attach(m_j_cancel);
    m_page_join->attach(m_j_continue);

    m_page_join->set_layout(
        column_layout(tuple { menu_width / 2, menu_width / 2 },
                      line_height, spacing));

    m_info->set_text(c_welcome);
    m_create->set_text(c_create);
    m_join->set_text(c_join);
    m_quit->set_text(c_quit);

    m_c_game_name_label->set_text(c_game_name);
    m_c_player_name_label->set_text(c_player_name);
    m_c_map_size_label->set_text(c_map_size);
    m_c_player_count_label->set_text(c_player_count);
    m_c_unit_count_label->set_text(c_unit_count);
    m_c_cancel->set_text(c_cancel);
    m_c_continue->set_text(c_continue);

    m_j_server_ip_label->set_text(c_server_ip);
    m_j_game_name_label->set_text(c_game_name);
    m_j_player_name_label->set_text(c_player_name);
    m_j_cancel->set_text(c_cancel);
    m_j_continue->set_text(c_continue);

    auto num_filter = [](char32_t c) -> bool {
      return c >= '0' && c <= '9';
    };

    m_c_map_size->setup_filter(num_filter);
    m_c_map_size->set_length_limit(4);
    m_c_player_count->setup_filter(num_filter);
    m_c_player_count->set_length_limit(3);
    m_c_unit_count->setup_filter(num_filter);
    m_c_unit_count->set_length_limit(3);

    m_j_server_ip->setup_filter([](char32_t c) -> bool {
      return (c >= '0' && c <= '9') || c == '.' || c == ':';
    });

    m_page_create->set_visible(false);
    m_page_join->set_visible(false);

    m_create->on_click([this](ptr_widget const &) {
      m_current_page = page::create;

      m_page_root->set_visible(false);
      m_page_create->set_visible(true);

      m_info->set_text(c_create_game);
    });

    m_join->on_click([this](ptr_widget const &) {
      m_current_page = page::join;

      m_page_root->set_visible(false);
      m_page_join->set_visible(true);

      m_info->set_text(c_join_game);

      const auto addr = as_ascii_string(m_j_server_ip->get_text());
      const auto host = session::get_host_address(addr, log);

      m_j_server_ip->set_text(as_u8string(host));
    });

    m_c_cancel->on_click([this](ptr_widget const &) {
      m_current_page = page::root;

      m_page_root->set_visible(true);
      m_page_create->set_visible(false);

      m_info->set_text(c_welcome);
    });

    m_j_cancel->on_click([this](ptr_widget const &) {
      m_current_page = page::root;

      m_page_root->set_visible(true);
      m_page_join->set_visible(false);

      m_info->set_text(c_welcome);
    });
  }

  mainmenu::~mainmenu() {
    if (auto p = m_info->get_parent(); p) {
      p->detach(m_info);
      p->detach(m_page_root);
      p->detach(m_page_create);
      p->detach(m_page_join);
    }
  }

  void mainmenu::on_create(mainmenu::event_create const &ev) {
    m_c_continue->on_click([this, ev](ptr_widget const &) {
      create_info info;

      info.game_name    = m_c_game_name->get_text();
      info.player_name  = m_c_player_name->get_text();
      info.map_size     = to_uint(m_c_map_size->get_text());
      info.player_count = to_uint(m_c_player_count->get_text());
      info.unit_count   = to_uint(m_c_unit_count->get_text());

      ev(info);
    });
  }

  void mainmenu::on_join(mainmenu::event_join const &ev) {
    m_j_continue->on_click([this, ev](ptr_widget const &) {
      join_info info;

      info.server_ip   = to_string(m_j_server_ip->get_text());
      info.game_name   = m_j_game_name->get_text();
      info.player_name = m_j_player_name->get_text();

      ev(info);
    });
  }

  void mainmenu::on_quit(mainmenu::event_quit const &ev) {
    m_quit->on_click([ev](ptr_widget const &) { ev(); });
  }

  void mainmenu::refresh() {
    m_current_page = page::root;

    if (m_is_visible) {
      m_page_root->set_visible(true);
      m_page_create->set_visible(false);
      m_page_join->set_visible(false);
    }

    m_info->set_text(c_welcome);
  }

  void mainmenu::set_visible(bool is_visible) {
    if (m_is_visible != is_visible) {
      m_is_visible = is_visible;
      m_info->set_visible(is_visible);

      switch (m_current_page) {
        case page::root: m_page_root->set_visible(is_visible); break;

        case page::create:
          m_page_create->set_visible(is_visible);
          break;

        case page::join: m_page_join->set_visible(is_visible); break;
      }
    }
  }

  void mainmenu::set_server_address(u8string_view ip) {
    m_j_server_ip->set_text(ip);
    m_j_server_ip->set_cursor(ip.size());
  }

  void mainmenu::set_game_name(u8string_view game_name) {
    m_c_game_name->set_text(game_name);
    m_c_game_name->set_cursor(game_name.size());

    m_j_game_name->set_text(game_name);
    m_j_game_name->set_cursor(game_name.size());
  }

  void mainmenu::set_player_name(u8string_view player_name) {
    m_c_player_name->set_text(player_name);
    m_c_player_name->set_cursor(player_name.size());

    m_j_player_name->set_text(player_name);
    m_j_player_name->set_cursor(player_name.size());
  }

  void mainmenu::set_map_size(size_t map_size) {
    const auto s = fmt(u8"%zu", map_size);

    m_c_map_size->set_text(s);
    m_c_map_size->set_cursor(s.size());
  }

  void mainmenu::set_player_count(size_t player_count) {
    const auto s = fmt(u8"%zu", player_count);

    m_c_player_count->set_text(s);
    m_c_player_count->set_cursor(s.size());
  }

  void mainmenu::set_unit_count(size_t unit_count) {
    const auto s = fmt(u8"%zu", unit_count);

    m_c_unit_count->set_text(s);
    m_c_unit_count->set_cursor(s.size());
  }

  void mainmenu::attach_to(ptr_widget const &w) {
    if (w) {
      w->attach(m_info);

      w->attach(m_page_root);
      w->attach(m_page_create);
      w->attach(m_page_join);
    }
  }

  void mainmenu::adjust_layout(sl::whole width, sl::whole height) {
    auto const lines_height = line_height * line_count;
    auto const full_height  = lines_height + info_height;

    auto const x0 = (width - menu_width) / 2;
    auto const y0 = (height - full_height) / 2;

    m_info->set_rect(rect { x0 + spacing, y0 + spacing,
                            menu_width - spacing * 2,
                            info_height - spacing * 2 });

    auto const lines_rect = rect { x0, y0 + info_height, menu_width,
                                   lines_height };

    m_page_root->set_rect(lines_rect);
    m_page_create->set_rect(lines_rect);
    m_page_join->set_rect(lines_rect);
  }
}
