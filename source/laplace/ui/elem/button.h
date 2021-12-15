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
  using event_button_click = std::function<void(ptr_widget const &)>;

  class button : public panel {
  public:
    struct update_result {
      bool is_pressed = false;
      bool has_cursor = false;
    };

    button(button const &) noexcept = default;
    button(button &&) noexcept      = default;
    auto operator=(button const &) noexcept -> button & = default;
    auto operator=(button &&) noexcept -> button & = default;

    button() noexcept           = default;
    ~button() noexcept override = default;

    void on_click(event_button_click ev) noexcept;

    void tick(sl::time                 delta_msec,
              core::cref_input_handler in) noexcept final;

    void render(context const &con) noexcept override;

    void set_pressed(bool is_pressed) noexcept;
    void set_cursor(bool has_cursor) noexcept;

    auto is_pressed() const noexcept -> bool;
    auto has_cursor() const noexcept -> bool;

    auto get_button_state() const noexcept -> button_state;

    static auto update(ptr_widget const         &object,
                       button_state              button_state,
                       event_button_click const &on_button_click,
                       core::cref_input_handler  in) noexcept
        -> update_result;

  private:
    void button_tick(core::cref_input_handler in) noexcept;

    event_button_click m_on_click;

    bool m_is_pressed = false;
    bool m_has_cursor = false;
  };

  using ptr_button = std::shared_ptr<button>;
}

#endif
