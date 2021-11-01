/*  laplace/ui/elem/textarea.h
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

#ifndef laplace_ui_elem_textarea_h
#define laplace_ui_elem_textarea_h

#include "../basic_widget.h"

namespace laplace::ui::elem {
  class textarea final : public basic_widget {
  public:
    static const sl::whole default_line_height;

    textarea(textarea const &) noexcept = default;
    textarea(textarea &&) noexcept      = default;
    auto operator=(textarea const &) noexcept -> textarea & = default;
    auto operator=(textarea &&) noexcept -> textarea & = default;

    textarea() noexcept        = default;
    ~textarea() noexcept final = default;

    void render(context const &con) noexcept final;

    void set_text(std::u8string_view text) noexcept;
    void set_line_height(int line_height) noexcept;

  private:
    void textarea_render(context const &con) noexcept;

    std::u8string m_text;
    sl::whole     m_line_height = default_line_height;
  };

  using ptr_textarea = std::shared_ptr<textarea>;
}

#endif
