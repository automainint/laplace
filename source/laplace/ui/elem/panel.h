/*  laplace/ui/elem/panel.h
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

#ifndef laplace_ui_elem_panel_h
#define laplace_ui_elem_panel_h

#include "../widget.h"

namespace laplace::ui::elem {
  class panel : public widget {
  public:
    struct state {
      sl::index level = 0;
      ui::rect  rect;
      bool      is_enabled = true;
    };

    ~panel() override = default;

    void set_context(ptr_context cont);

    void render() override;

    auto get_state() const -> state;

  protected:
    ptr_context m_context = widget::get_default_context();
  };

  using ptr_panel = std::shared_ptr<panel>;
}

#endif
