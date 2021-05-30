/*  apps/quadwar/quadwar.h
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

#ifndef quadwar_quadwar_h
#define quadwar_quadwar_h

#include "../../laplace/stem/app_flat.h"
#include "session.h"
#include "ui/mainmenu.h"

namespace quadwar_app {
  static constexpr auto log_file_name = "quadwar.log";

  static constexpr auto k_server_address = "server_address";
  static constexpr auto k_game_name      = "game_name";
  static constexpr auto k_player_name    = "player_name";
  static constexpr auto k_map_size       = "map_size";
  static constexpr auto k_player_count   = "player_count";
  static constexpr auto k_unit_count     = "unit_count";

  static constexpr auto default_player_names = {
    u8"Alice",     u8"Bob",     u8"Foo",      u8"Bar",
    u8"Newbye",    u8"Gosu",    u8"Decartes", u8"Socrates",
    u8"Spinosa",   u8"Pluto",   u8"Monke",    u8"Deleuze",
    u8"Sorceress", u8"Daemon",  u8"Warlock",  u8"Ranger",
    u8"Priest",    u8"Doge",    u8"Glider",   u8"Skeleton",
    u8"Peon",      u8"Baron",   u8"Warlord",  u8"Count",
    u8"Point",     u8"Triangle"
  };

  static constexpr auto default_server_address = "127.0.0.1";
  static constexpr auto default_game_name      = "Unknown";
  static constexpr auto default_player_count   = 4u;
  static constexpr auto default_unit_count     = 8u;
  static constexpr auto default_map_size       = 128u;

  class quadwar : public stem::app_flat {
  public:
    static constexpr auto caption = u8"Quadwar";

    quadwar(int argc, char **argv);
    ~quadwar() override = default;

  protected:
    void init() override;
    void cleanup() override;
    void update(uint64_t delta_msec) override;
    void render() override;
    void adjust_layout(int width, int height) override;

  private:
    static auto get_player_name() -> std::u8string_view;
    static auto get_config() -> core::family;

    ui::ptr_mainmenu m_mainmenu;
    ptr_session      m_session;
    bool             m_return_to_mainmenu = false;
  };
}

#endif
