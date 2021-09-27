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

#include "../widget.h"

namespace laplace::ui::elem {
  class textarea final : public widget {
  public:
    static const sl::whole default_line_height;

    ~textarea() final = default;

    void render(context const &con) final;

    void set_text(std::u8string_view text);
    void set_line_height(int line_height);

  private:
    void textarea_render(context const &con);

    std::u8string m_text;
    sl::whole     m_line_height = default_line_height;
  };

  using ptr_textarea  = std::shared_ptr<textarea>;
  using vptr_textarea = sl::vector<ptr_textarea>;
}

#endif
