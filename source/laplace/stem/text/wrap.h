/*  laplace/ui/text/wrap.h
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

#ifndef laplace_stem_text_wrap_h
#define laplace_stem_text_wrap_h

#include "../../graphics/texture.h"
#include "../../render/context.h"
#include "painter.h"
#include "renderer.h"

namespace laplace::stem::text {
  /*  Painter to Renderer wrapper.
   */
  class wrap final : public renderer {
  public:
    wrap(ptr_painter paint);
    ~wrap() override = default;

    void set_render_context(render::ptr_context cont);

    auto adjust(std::u8string_view text) -> ui::text_area final;

    void render(sl::index          x,
                sl::index          y,
                std::u8string_view text) final;

  private:
    render::ptr_context m_render = render::context::get_default();
    ptr_painter         m_paint;

    graphics::ptr_texture m_tex =
        std::make_shared<graphics::texture>();
  };
}

#endif
