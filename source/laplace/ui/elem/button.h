/*  laplace/ui/elem/button.h
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

#ifndef laplace_ui_elem_button_h
#define laplace_ui_elem_button_h

#include "panel.h"

namespace laplace::ui::elem {
  using event_button_click = std::function<void(ptr_widget)>;

  class button : public panel {
  public:
    struct update_result {
      bool event_status;
      bool is_pressed;
      bool has_cursor;
    };

    ~button() override = default;

    void on_click(event_button_click ev);

    auto tick(sl::time                 delta_msec,
              core::cref_input_handler in,
              bool                     is_handled) -> bool final;

    void render(context const &con) override;

    void set_pressed(bool is_pressed);
    void set_cursor(bool has_cursor);

    auto is_pressed() const -> bool;
    auto has_cursor() const -> bool;

    auto get_state() const -> button_state;

    static auto update(ptr_widget               object,
                       button_state             button_state,
                       event_button_click       on_button_click,
                       core::cref_input_handler in) -> update_result;

  private:
    auto button_tick(core::cref_input_handler in) -> bool;

    event_button_click m_on_click;

    bool m_is_pressed = false;
    bool m_has_cursor = false;
  };

  using ptr_button = std::shared_ptr<button>;
}

#endif
