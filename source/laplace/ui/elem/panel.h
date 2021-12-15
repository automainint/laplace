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

#include "../basic_widget.h"

namespace laplace::ui::elem {
  class panel : public basic_widget {
  public:
    panel(panel const &) noexcept = default;
    panel(panel &&) noexcept      = default;
    auto operator=(panel const &) noexcept -> panel & = default;
    auto operator=(panel &&) noexcept -> panel & = default;

    panel() noexcept           = default;
    ~panel() noexcept override = default;

    void render(context const &con) noexcept override;

    auto get_panel_state() const noexcept -> panel_state;
  };

  using ptr_panel = std::shared_ptr<panel>;
}

#endif
