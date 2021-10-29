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
    ~panel() override = default;

    void render(context const &con) override;

    auto get_state() const -> panel_state;
  };

  using ptr_panel = std::shared_ptr<panel>;
}

#endif
