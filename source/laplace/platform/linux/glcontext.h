/*  laplace/platform/glcontext.h
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

#ifndef laplace_platform_linux_glcontext_h
#define laplace_platform_linux_glcontext_h

#include "window.h"

namespace laplace::linux {
  class glcontext {
  public:
    glcontext(glcontext const &) = delete;
    glcontext(glcontext &&)      = delete;
    auto operator=(glcontext const &) -> glcontext & = delete;
    auto operator=(glcontext &&) -> glcontext & = delete;

    explicit glcontext(std::shared_ptr<window> win) noexcept;
    ~glcontext() noexcept = default;

    void        swap_buffers() noexcept;
    static void set_forward_compatible(bool) noexcept;

  private:
    std::shared_ptr<window> m_window;
  };
}

#endif
