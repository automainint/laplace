/*  laplace/ui/elem/textbutton.h
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

#ifndef laplace_ui_elem_textbutton_h
#define laplace_ui_elem_textbutton_h

#include "button.h"

namespace laplace::ui::elem {
  class textbutton final : public button {
  public:
    ~textbutton() final = default;

    void render() final;

    void set_text(std::u8string_view text);

    auto get_state() const -> textbutton_state;

  private:
    std::u8string m_text;
  };

  using ptr_textbutton = std::shared_ptr<textbutton>;
}

#endif
