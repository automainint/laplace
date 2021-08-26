/*  laplace/ui/text/uit_renderer.cpp
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

#include "renderer.h"
#include "wrap.h"

namespace laplace::ui::text {
  using std::make_shared, std::weak_ptr, std::u8string_view;

  weak_ptr<renderer> renderer::m_default;

  auto renderer::adjust(u8string_view text) -> renderer::area {
    return {};
  }

  void renderer::render(
      sl::index x, sl::index y, u8string_view text) { }

  auto renderer::get_default() -> ptr_renderer {
    auto p = m_default.lock();

    if (!p) {
      p = make_shared<wrap>(painter::get_default());

      m_default = p;
    }

    return p;
  }
}
