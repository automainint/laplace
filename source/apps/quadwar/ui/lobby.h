/*  apps/quadwar/ui/lobby.h
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

#ifndef quadwar_ui_lobby_h
#define quadwar_ui_lobby_h

#include "defs.h"

namespace quadwar_app::ui {
  class lobby : helper {
  public:
    using event_abort = std::function<void()>;
    using event_start = std::function<void()>;

    static sl::whole const spacing;
    static sl::whole const side_size;
    static sl::whole const button_width;
    static sl::whole const line_height;

    static std::u8string_view const c_abort;
    static std::u8string_view const c_start;
    static std::u8string_view const c_slot_open;
    static std::u8string_view const c_status_loading;

    lobby();
    ~lobby();

    void on_abort(event_abort ev);
    void on_start(event_start ev);

    void attach_to(ui::ptr_widget w);
    void adjust_layout(sl::whole width, sl::whole height);

    void show_info(std::u8string_view text);
    void show_loading();

    void set_slot(size_t             index,
                  size_t             id_actor,
                  std::u8string_view name);

    void remove_slot(size_t index);

    auto get_slot_count() const -> size_t;

    void set_visible(bool is_visible);
    void set_start_enabled(bool is_enabled);

    auto is_visible() const -> bool;

  private:
    struct slot {
      size_t       id_actor = -1;
      ptr_textarea ui_name  = make<textarea>();
    };

    bool              m_is_visible = true;
    std::vector<slot> m_slots;

    ptr_textarea   m_info       = make<textarea>();
    ptr_widget     m_slots_area = make<widget>();
    ptr_textbutton m_abort      = make<textbutton>();
    ptr_textbutton m_start      = make<textbutton>();
  };
}

#endif
