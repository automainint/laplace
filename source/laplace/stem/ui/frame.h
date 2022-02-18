/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_STEM_UI_FRAME_H
#define LAPLACE_STEM_UI_FRAME_H

#include "../../graphics/flat/framebuffer.h"
#include "../../render/context.h"
#include "../../ui/basic_widget.h"

namespace laplace::stem::ui {
  class frame final : public laplace::ui::basic_widget {
  public:
    log_handler log = get_global_log();
    
    frame(frame const &) = delete;
    frame(frame &&)      = delete;
    auto operator=(frame const &) = delete;
    auto operator=(frame &&) = delete;

    frame() noexcept        = default;
    ~frame() noexcept final = default;

    void set_render_context(render::ptr_context con) noexcept;

    void render(laplace::ui::context const &con) noexcept final;

  private:
    render::ptr_context m_render = render::context::get_default();
    graphics::flat::framebuffer m_buffer;
  };

  using ptr_frame = std::shared_ptr<frame>;
}

#endif
