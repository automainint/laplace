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
      bool          event_status = false;
      bool          has_focus    = false;
      std::u8string text;
      sl::index     cursor    = 0;
      sl::index     selection = 0;
    };

    static constexpr size_t default_length_limit = 24;

    textedit();
    ~textedit() final = default;

    void setup_filter(filter f);

    auto tick(sl::time                 delta_msec,
              core::cref_input_handler in,
              bool                     is_handled) -> bool final;

    void render() final;

    void set_text(std::u8string_view text);
    void set_length_limit(sl::whole limit);
    void set_cursor(sl::index cursor);
    void set_selection(sl::index selection);

    auto get_text() const -> std::u8string_view;
    auto get_cursor() const -> sl::index;
    auto get_selection() const -> sl::index;

    auto get_state() const -> textedit_state;

    static auto update(ptr_widget               object,
                       textedit_state           state,
                       filter                   f,
                       core::input_event const &ev) -> update_result;

  private:
    auto textedit_tick(core::cref_input_handler in) -> bool;

    filter        m_filter;
    sl::whole     m_length_limit = default_length_limit;
    std::u8string m_text;
    sl::index     m_cursor    = 0;
    sl::index     m_selection = 0;
  };

  using ptr_textedit = std::shared_ptr<textedit>;
}

#endif
