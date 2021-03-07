/*  laplace/graphics/flat/framebuffer.h
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

#ifndef laplace_graphics_flat_framebuffer_h
#define laplace_graphics_flat_framebuffer_h

#include "../texture.h"
#include <functional>

namespace laplace::graphics::flat {
  class framebuffer {
  public:
    texture color_texture;

    framebuffer();
    ~framebuffer();

    void set_size(size_t width, size_t height);

    void render(std::function<void()> op);

  private:
    uint32_t m_id;
    uint32_t m_width;
    uint32_t m_height;
  };

  using ptr_framebuffer = std::shared_ptr<framebuffer>;
}

#endif
