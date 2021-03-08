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
#include "../../laplace/platform/wrap.h"
#include "protocol/ids.h"
#include "qw_factory.h"
#include "ui/lobby.h"
#include "view/game.h"

namespace quadwar_app {
  class session {
  public:
    using event_done = std::function<void()>;
    using event_quit = std::function<void()>;

    static constexpr auto sense_move  = 1.5f;
    static constexpr auto sense_scale = .0003f;

    static constexpr auto host_info_file = ".host";

    static constexpr uint16_t allowed_commands[] = {
      protocol::ids::public_key,   protocol::ids::request_events,
      protocol::ids::ping_request, protocol::ids::ping_response,
      protocol::ids::client_enter, protocol::ids::client_leave,
      protocol::ids::client_ready, protocol::ids::player_name
    };

    static constexpr auto     default_server_ip = "127.0.0.1";
    static constexpr uint16_t default_port      = network::any_port;

    session();
    ~session();

    void on_done(event_done ev);
    void on_quit(event_quit ev);

    void tick(uint64_t delta_msec, platform::ref_input in);
    void render();

    void attach_to(ui::ptr_widget w);
    void adjust_layout(int width, int height);

    void set_server_ip(std::string_view server_ip);
    void set_server_port(uint16_t port);
    void set_game_name(std::u8string_view game_name);
    void set_player_name(std::u8string_view player_name);
    void set_map_size(size_t map_size);
    void set_player_count(size_t player_count);
    void set_unit_count(size_t unit_count);

    void create();
    void join();

    [[nodiscard]] static auto get_host_address(
        std::string_view default_address) -> std::string;

  private:
    void update_control(uint64_t delta_msec, platform::ref_input in);
    void update_lobby();
    void save_host_info(uint16_t port);

    event_done m_on_done;
    event_quit m_on_quit;

    /*  Local actor id.
     */
    size_t m_id_actor = engine::id_undefined;

    engine::ptr_world m_world;

    std::string   m_server_ip   = default_server_ip;
    uint16_t      m_server_port = default_port;
    std::u8string m_game_name;
    std::u8string m_player_name;
    size_t        m_map_size        = 0;
    size_t        m_player_count    = 0;
    size_t        m_unit_count      = 0;
    bool          m_host_info_saved = false;

    network::ptr_server m_server;

    view::game m_view;

    ui::lobby m_lobby;
  };

  using ptr_session = std::shared_ptr<session>;
}

#endif
