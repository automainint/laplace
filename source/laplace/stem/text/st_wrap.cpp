/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "wrap.h"

namespace laplace::stem::text {
  using sprite_vertex = render::context::sprite_vertex;
  using std::u8string_view, graphics::image, ui::to_rectf;

  wrap::wrap(ptr_painter paint) {
    m_paint = paint;
  }

  void wrap::set_render_context(render::ptr_context cont) {
    m_render = cont;
  }

  auto wrap::adjust(u8string_view text) -> ui::text_area {
    return m_paint ? m_paint->adjust(text) : ui::text_area {};
  }

  void wrap::render(sl::index x, sl::index y, u8string_view text) {
    if constexpr (!_unsafe) {
      if (!m_paint) {
        log(log_event::error, "No painter.", __FUNCTION__);
        return;
      }
      if (!m_tex) {
        log(log_event::error, "No texture.", __FUNCTION__);
        return;
      }
      if (!m_render) {
        log(log_event::error, "No render context.", __FUNCTION__);
        return;
      }
    }

    auto img = image {};
    auto a   = m_paint->adjust(text);

    img.set_size(a.width, a.height);

    m_paint->print(img, 0, a.top, text);
    m_tex->image_2d_nearest(img);

    const auto f = to_rectf(
        { .x = x, .y = y, .width = a.width, .height = a.height });

    const auto v = {
      sprite_vertex { .position = { f.x, f.y },
                      .texcoord = { 0.f, 1.f } },
      sprite_vertex { .position = { f.x + f.width, f.y },
                      .texcoord = { 1.f, 1.f } },
      sprite_vertex { .position = { f.x, f.y + f.height },
                      .texcoord = { 0.f, 0.f } },
      sprite_vertex { .position = { f.x + f.width, f.y + f.height },
                      .texcoord = { 1.f, 0.f } }
    };

    m_render->render_strip(v, *m_tex);
  }
}
