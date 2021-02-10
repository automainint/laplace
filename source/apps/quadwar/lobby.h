/*  apps/quadwar/lobby.h
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

#ifndef __quadwar__lobby__
#define __quadwar__lobby__

#include "../../laplace/ui/helper.h"

namespace quadwar_app {
  using namespace laplace;

  class lobby : ui::helper {
  public:
    using event_abort = std::function<void()>;
    using event_start = std::function<void()>;

    static constexpr int spacing      = 4;
    static constexpr int side_size    = 16;
    static constexpr int button_width = 260;
    static constexpr int line_height  = 60;

    static constexpr auto c_abort = u8"Abort";
    static constexpr auto c_start = u8"Start";

    lobby();
    ~lobby();

    void on_abort(event_abort ev);
    void on_start(event_start ev);

    void attach_to(ui::ptr_widget w);
    void adjust_layout(int width, int height);

    void show_info(std::u8string_view text);

    void set_slot(size_t index, size_t id_actor,
                  std::u8string_view name);

    void remove_slot(size_t index);

    auto get_slot_count() const -> size_t;

    void set_visible(bool is_visible);
    void set_start_enabled(bool is_enabled);

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
