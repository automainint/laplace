/*  apps/quadwar/session.h
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

#ifndef quadwar_session_h
#define quadwar_session_h

#include "../../laplace/network/defs.h"
#include "../../laplace/network/server.h"
#include "protocol/ids.h"
#include "qw_factory.h"
#include "ui/lobby.h"
#include "view/game.h"

namespace quadwar_app {
  class session {
  public:
    using event_done = std::function<void()>;
    using event_quit = std::function<void()>;

    static uint16_t const  allowed_commands[];
    static char const      host_info_file[];
    static char const      default_server_ip[];
    static uint16_t const  default_port;
    static sl::whole const thread_count;
    static float const     sense_move;
    static float const     sense_scale;

    session();
    ~session();

    void on_done(event_done ev);
    void on_quit(event_quit ev);

    void tick(sl::time delta_msec, core::cref_input_handler in);
    void render();

    void attach_to(ui::ptr_widget w);
    void adjust_layout(sl::whole width, sl::whole height);

    void set_server_ip(std::string_view server_ip);
    void set_server_port(uint16_t port);
    void set_game_name(std::u8string_view game_name);
    void set_player_name(std::u8string_view player_name);
    void set_map_size(sl::whole map_size);
    void set_player_count(sl::whole player_count);
    void set_unit_count(sl::whole unit_count);

    void create();
    void join();

    [[nodiscard]] static auto get_host_address(
        std::string_view default_address) -> std::string;

  private:
    void update_control(sl::time                 delta_msec,
                        core::cref_input_handler in);
    void update_lobby();
    void save_host_info(uint16_t port);

    event_done m_on_done;
    event_quit m_on_quit;

    /*  Local actor id.
     */
    sl::index m_id_actor = engine::id_undefined;
    sl::index m_root_ver = 0;

    engine::ptr_world m_world;

    std::string   m_server_ip   = default_server_ip;
    uint16_t      m_server_port = default_port;
    std::u8string m_game_name;
    std::u8string m_player_name;
    sl::whole     m_map_size        = 0;
    sl::whole     m_player_count    = 0;
    sl::whole     m_unit_count      = 0;
    bool          m_host_info_saved = false;
    bool          m_show_game       = false;

    network::ptr_server m_server;

    view::game m_view;

    ui::lobby m_lobby;
  };

  using ptr_session = std::shared_ptr<session>;
}

#endif
