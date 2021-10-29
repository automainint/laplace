/*  laplace/stem/text/renderer.h
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

#ifndef laplace_stem_text_renderer_h
#define laplace_stem_text_renderer_h

#include "painter.h"

namespace laplace::stem::text {
  /*  Rendering text as vertex array.
   */
  class renderer {
  public:
    using ptr_renderer = std::shared_ptr<renderer>;

    virtual ~renderer() = default;

    virtual auto adjust(std::u8string_view text) -> ui::text_area;

    virtual void render(
        sl::index x, sl::index y, std::u8string_view text);

    static auto get_default() -> ptr_renderer;

  private:
    static std::weak_ptr<renderer> m_default;
  };

  using ref_renderer  = renderer &;
  using cref_renderer = const renderer &;

  using ptr_renderer = renderer::ptr_renderer;
}

#endif
