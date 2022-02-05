/*  Copyright (c) 2022 Mitya Selivanov
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
    button(button const &) noexcept = default;
    button(button &&) noexcept      = default;
    auto operator=(button const &) noexcept -> button & = default;
    auto operator=(button &&) noexcept -> button & = default;

    button() noexcept           = default;
    ~button() noexcept override = default;

    void on_click(event_button_click ev) noexcept;

    auto process_event(core::input_event const &ev) noexcept
        -> event_status final;

    void tick(sl::time                   delta_msec,
              core::input_handler const &in) noexcept final;

    void render(context const &con) noexcept override;

    void set_pressed(bool is_pressed) noexcept;
    void set_cursor(bool has_cursor) noexcept;

    [[nodiscard]] auto is_pressed() const noexcept -> bool;
    [[nodiscard]] auto has_cursor() const noexcept -> bool;

    [[nodiscard]] auto get_button_state() const noexcept
        -> button_state;

    struct update_result {
      bool is_pressed = false;
    };

    [[nodiscard]] static auto update(
        ptr_widget const &object, button_state button_state,
        event_button_click const &on_button_click,
        core::input_event const  &ev) noexcept -> update_result;

  private:
    auto button_process_event(core::input_event const &ev) noexcept
        -> event_status;

    void button_tick(core::input_handler const &in) noexcept;

    event_button_click m_on_click;

    bool m_is_pressed = false;
    bool m_has_cursor = false;
  };
}

#endif
