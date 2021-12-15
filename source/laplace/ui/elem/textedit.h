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

    struct update_result {
      bool          has_focus = false;
      std::u8string text;
      sl::index     cursor    = 0;
      sl::index     selection = 0;
    };

    static constexpr size_t default_length_limit = 24;

    textedit(textedit const &) noexcept = default;
    textedit(textedit &&) noexcept      = default;
    auto operator=(textedit const &) noexcept -> textedit & = default;
    auto operator=(textedit &&) noexcept -> textedit & = default;

    textedit() noexcept;
    ~textedit() noexcept final = default;

    void setup_filter(filter f) noexcept;

    void tick(sl::time                 delta_msec,
              core::cref_input_handler in) noexcept final;

    void render(context const &con) noexcept final;

    void set_text(std::u8string_view text) noexcept;
    void set_length_limit(sl::whole limit) noexcept;
    void set_cursor(sl::index cursor) noexcept;
    void set_selection(sl::index selection) noexcept;

    auto get_text() const noexcept -> std::u8string_view;
    auto get_cursor() const noexcept -> sl::index;
    auto get_selection() const noexcept -> sl::index;

    auto get_textedit_state() const noexcept -> textedit_state;

    static auto update(ptr_widget const        &object,
                       textedit_state           state,
                       filter const            &f,
                       core::input_event const &ev) noexcept
        -> update_result;

  private:
    void textedit_tick(core::cref_input_handler in) noexcept;

    filter        m_filter;
    sl::whole     m_length_limit = default_length_limit;
    std::u8string m_text;
    sl::index     m_cursor    = 0;
    sl::index     m_selection = 0;
  };

  using ptr_textedit = std::shared_ptr<textedit>;
}

#endif
