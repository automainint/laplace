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

  class quadwar : public stem::app_flat {
  public:
    static const char8_t * default_player_names[];
    static const char *    default_server_address;
    static const char *    default_game_name;
    static const sl::whole default_player_count;
    static const sl::whole default_unit_count;
    static const sl::whole default_map_size;

    static const char8_t *caption;

    quadwar(int argc, char **argv);
    ~quadwar() override = default;

  protected:
    void init() override;
    void cleanup() override;
    void update(uint64_t delta_msec) override;
    void render() override;
    void adjust_layout(sl::whole width, sl::whole height) override;

  private:
    static auto get_player_name() -> std::u8string_view;
    static auto get_config() -> core::family;

    ui::ptr_mainmenu m_mainmenu;
    ptr_session      m_session;
    bool             m_return_to_mainmenu = false;
  };
}

#endif
