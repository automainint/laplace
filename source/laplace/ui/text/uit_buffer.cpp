/*  laplace/ui/text/uit_buffer.cpp
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

#include "buffer.h"

namespace laplace::ui::text {
  using std::u8string_view, graphics::cref_image;

  void buffer::build(ref_painter paint) {
    /*  TODO
     */
  }

  void buffer::build_monospace(ref_painter paint, sl::index top,
                               sl::index char_width,
                               sl::index char_height) {
    /*  TODO
     */
  }

  void buffer::build_monospace(cref_image img, sl::index top,
                               sl::index char_width,
                               sl::index char_height) {
    /*  TODO
     */
  }

  auto buffer::adjust(u8string_view text) -> renderer::area {
    /*  TODO
     */

    return { 0, 0, 0 };
  }

  void buffer::render(sl::index x, sl::index y, u8string_view text) {
    /*  TODO
     */
  }
}
