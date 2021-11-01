/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "glcontext.h"

#include <utility>

namespace laplace::linux {
  glcontext::glcontext(std::shared_ptr<window> win) noexcept :
      m_window(std::move(win)) {

    if (!gl_is_loaded())
      return;

    if (!gl::load_functions())
      error_("OpenGL initialization failed.", __FUNCTION__);
  }

  void glcontext::swap_buffers() noexcept {
    auto *display = m_window->get_display();
    auto  handle  = m_window->get_native_handle();

    if (display != nullptr)
      glXSwapBuffers(display, handle);
  }

  void glcontext::set_forward_compatible(bool) noexcept { }

}
