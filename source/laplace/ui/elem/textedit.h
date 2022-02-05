/*  laplace/ui/elem/textedit.h
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

#ifndef laplace_ui_elem_textedit_h
#define laplace_ui_elem_textedit_h

#include "panel.h"

namespace laplace::ui::elem {
  class textedit final : public panel {
  public:
    using filter = std::function<bool(char32_t c)>;

    static sl::whole default_length_limit;
    static sl::whole default_flash_on;
    static sl::whole default_flash_off;

    struct update_result {
      bool          has_focus = false;
      std::u8string text;
      sl::index     cursor    = 0;
      sl::index     selection = 0;
    };

    textedit(textedit const &) noexcept = default;
    textedit(textedit &&) noexcept      = default;
    auto operator=(textedit const &) noexcept -> textedit & = default;
    auto operator=(textedit &&) noexcept -> textedit & = default;

    textedit() noexcept;
    ~textedit() noexcept final = default;

    void setup_filter(filter f) noexcept;

    auto process_event(core::input_event const &ev) noexcept
        -> event_status final;

    void tick(sl::time delta_msec,
              core::input_handler const &) noexcept final;

    void render(context const &con) noexcept final;

    void set_text(std::u8string_view text) noexcept;
    void set_length_limit(sl::whole limit) noexcept;
    void set_cursor(sl::index cursor) noexcept;
    void set_selection(sl::index selection) noexcept;

    [[nodiscard]] auto get_text() const noexcept
        -> std::u8string_view;
    [[nodiscard]] auto get_length_limit() const noexcept -> sl::whole;
    [[nodiscard]] auto get_cursor() const noexcept -> sl::index;
    [[nodiscard]] auto get_selection() const noexcept -> sl::index;
    [[nodiscard]] auto get_flash() const noexcept -> sl::index;

    [[nodiscard]] auto get_textedit_state() const noexcept
        -> textedit_state;

    [[nodiscard]] static auto update(
        ptr_widget const &object, textedit_state state,
        filter const &f, core::input_event const &ev) noexcept
        -> update_result;

  private:
    void textedit_set_focus(bool has_focus) noexcept;
    void set_flash(sl::index flash) noexcept;

    void textedit_flash(sl::time delta_msec) noexcept;

    auto textedit_process_event(core::input_event const &ev) noexcept
        -> event_status;

    filter        m_filter;
    sl::whole     m_length_limit = default_length_limit;
    sl::whole     m_flash_on     = default_flash_on;
    sl::whole     m_flash_off    = default_flash_off;
    std::u8string m_text;
    sl::index     m_cursor    = 0;
    sl::index     m_selection = 0;
    sl::index     m_flash     = 0;
  };

  using ptr_textedit = std::shared_ptr<textedit>;
}

#endif
