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

#include "../../laplace/ui/elem/textarea.h"
#include "../../laplace/ui/elem/textbutton.h"
#include "../../laplace/ui/widget.h"

namespace quadwar_app {
  using namespace laplace;

  class lobby {
  public:
    using event_abort = std::function<void()>;
    using event_start = std::function<void()>;

    static constexpr size_t spacing      = 4;
    static constexpr size_t side_size    = 16;
    static constexpr size_t button_width = 260;
    static constexpr size_t line_height  = 60;

    static constexpr auto c_abort = u8"Abort";
    static constexpr auto c_start = u8"Start";

    lobby();
    ~lobby();

    void on_abort(event_abort ev);
    void on_start(event_start ev);

    void attach_to(ui::ptr_widget w);
    void adjust_layout(size_t width, size_t height);

    void show_info(std::u8string_view text);

    void set_slot(size_t index, size_t id_actor,
                  std::u8string_view name);

    void remove_slot(size_t index);

    void set_visible(bool is_visible);
    void set_start_enabled(bool is_enabled);

  private:
    struct slot {
      size_t                 id_actor = -1;
      ui::elem::ptr_textarea ui_name =
          std::make_shared<ui::elem::textarea>();
    };

    bool                   m_is_visible = true;
    std::vector<slot>      m_slots;
    ui::elem::ptr_textarea m_info =
        std::make_shared<ui::elem::textarea>();
    ui::ptr_widget m_slots_area =
        std::make_shared<ui::widget>();
    ui::elem::ptr_textbutton m_abort =
        std::make_shared<ui::elem::textbutton>();
    ui::elem::ptr_textbutton m_start =
        std::make_shared<ui::elem::textbutton>();
  };
}

#endif
