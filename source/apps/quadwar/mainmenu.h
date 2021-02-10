/*  apps/quadwar/mainmenu.h
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

#ifndef __quadwar__mainmenu__
#define __quadwar__mainmenu__

#include "../../laplace/ui/helper.h"

namespace quadwar_app {
  using namespace laplace;

  class mainmenu : ui::helper {
  public:
    struct create_info {
      std::u8string game_name;
      std::u8string player_name;
      size_t        map_size     = 0;
      size_t        player_count = 0;
      size_t        unit_count   = 0;
    };

    struct join_info {
      std::string   server_ip;
      std::u8string game_name;
      std::u8string player_name;
    };

    using event_create = std::function<void(create_info)>;
    using event_join   = std::function<void(join_info)>;
    using event_quit   = std::function<void()>;

    static constexpr int spacing     = 4;
    static constexpr int line_count  = 7;
    static constexpr int menu_width  = 640;
    static constexpr int info_height = 80;
    static constexpr int line_height = 60;

    static constexpr auto c_welcome      = u8"Welcome";
    static constexpr auto c_create       = u8"Create";
    static constexpr auto c_join         = u8"Join";
    static constexpr auto c_quit         = u8"Quit";
    static constexpr auto c_create_game  = u8"Create Game";
    static constexpr auto c_join_game    = u8"Join Game";
    static constexpr auto c_server_ip    = u8"Server IP";
    static constexpr auto c_game_name    = u8"Game name";
    static constexpr auto c_player_name  = u8"Player name";
    static constexpr auto c_map_size     = u8"Map size";
    static constexpr auto c_player_count = u8"Player count";
    static constexpr auto c_unit_count   = u8"Unit count";
    static constexpr auto c_cancel       = u8"Cancel";
    static constexpr auto c_continue     = u8"Continue";

    mainmenu();
    ~mainmenu();

    void on_create(event_create ev);
    void on_join(event_join ev);
    void on_quit(event_quit ev);

    void refresh();

    void set_visible(bool is_visible);

    void set_server_address(std::u8string_view ip);
    void set_game_name(std::u8string_view game_name);
    void set_player_name(std::u8string_view player_name);
    void set_map_size(size_t map_size);
    void set_player_count(size_t player_count);
    void set_unit_count(size_t unit_count);

    void attach_to(ui::ptr_widget w);
    void adjust_layout(int width, int height);

  private:
    enum class page { root, create, join };

    page m_current_page = page::root;
    bool m_is_visible   = true;

    ptr_textarea m_info        = make<textarea>();
    ptr_widget   m_page_root   = make<widget>();
    ptr_widget   m_page_create = make<widget>();
    ptr_widget   m_page_join   = make<widget>();

    ptr_textbutton m_create = make<textbutton>();
    ptr_textbutton m_join   = make<textbutton>();
    ptr_textbutton m_quit   = make<textbutton>();

    ptr_textarea m_c_game_name_label    = make<textarea>();
    ptr_textarea m_c_player_name_label  = make<textarea>();
    ptr_textarea m_c_map_size_label     = make<textarea>();
    ptr_textarea m_c_player_count_label = make<textarea>();
    ptr_textarea m_c_unit_count_label   = make<textarea>();

    ptr_textedit m_c_game_name    = make<textedit>();
    ptr_textedit m_c_player_name  = make<textedit>();
    ptr_textedit m_c_map_size     = make<textedit>();
    ptr_textedit m_c_player_count = make<textedit>();
    ptr_textedit m_c_unit_count   = make<textedit>();

    ptr_textbutton m_c_cancel   = make<textbutton>();
    ptr_textbutton m_c_continue = make<textbutton>();

    ptr_textarea m_j_server_ip_label   = make<textarea>();
    ptr_textarea m_j_game_name_label   = make<textarea>();
    ptr_textarea m_j_player_name_label = make<textarea>();

    ptr_textedit m_j_server_ip   = make<textedit>();
    ptr_textedit m_j_game_name   = make<textedit>();
    ptr_textedit m_j_player_name = make<textedit>();

    ptr_textbutton m_j_cancel   = make<textbutton>();
    ptr_textbutton m_j_continue = make<textbutton>();
  };

  using ptr_mainmenu = std::shared_ptr<mainmenu>;
}

#endif
