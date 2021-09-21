/*  laplace/ui/elem/canvas.h
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

#ifndef laplace_ui_elem_canvas_h
#define laplace_ui_elem_canvas_h

#include "../../graphics/texture.h"
#include "../widget.h"

namespace laplace::ui::elem {
  class canvas final : public widget {
  public:
    canvas();
    ~canvas() final = default;

    void set_context(ptr_context cont);

    void render() final;

    auto image() -> graphics::ref_image;
    auto get_image() const -> graphics::cref_image;

  private:
    graphics::image   m_image;
    graphics::texture m_texture;

    ptr_context m_context;
  };

  using ptr_canvas = std::shared_ptr<canvas>;
}

#endif
