#pragma once

#include "ids.h"
#include "lobby.h"
#include "qw_factory.h"
#include "qw_slot_create.h"
#include "root.h"
#include <laplace/engine/server.h>

namespace quadwar_app {
  using namespace laplace;
  using namespace engine;

  class session {
  public:
    using event_done = std::function<void()>;
    using event_quit = std::function<void()>;

    static constexpr uint16_t allowed_commands[] = {
      ids::public_key, ids::request_events, ids::client_ping,
      ids::client_enter, ids::client_leave, ids::client_ready,
      ids::slot_create, ids::slot_remove
    };

    static constexpr auto     default_server_ip = "127.0.0.1";
    static constexpr uint16_t default_port      = 0;

    void on_done(event_done ev);
    void on_quit(event_quit ev);

    void tick(size_t delta_msec);

    void attach_to(ui::ptr_widget w);
    void adjust_layout(size_t width, size_t height);

    void set_server_ip(std::string_view server_ip);
    void set_server_port(uint16_t port);
    void set_game_name(std::u8string_view game_name);
    void set_player_name(std::u8string_view player_name);
    void set_map_size(size_t map_size);
    void set_player_count(size_t player_count);
    void set_unit_count(size_t unit_count);

    void create();
    void join();

    void cleanup();

  private:
    /*  Local actor id.
     */
    size_t m_id_actor = -1;

    ptr_world m_world;
    ptr_root  m_root;

    std::string   m_server_ip   = default_server_ip;
    uint16_t      m_server_port = default_port;
    std::u8string m_game_name;
    std::u8string m_player_name;
    size_t        m_map_size     = 0;
    size_t        m_player_count = 0;
    size_t        m_unit_count   = 0;

    ptr_factory m_factory = std::make_shared<qw_factory>();
    ptr_server  m_server;

    lobby m_lobby;
  };

  using ptr_session = std::shared_ptr<session>;
}
