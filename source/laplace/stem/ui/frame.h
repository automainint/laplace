/*  laplace/ui/frame.h
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

#ifndef laplace_stem_ui_frame_h
#define laplace_stem_ui_frame_h

#include "../../graphics/flat/framebuffer.h"
#include "../../render/context.h"
#include "../../ui/widget.h"

namespace laplace::stem::ui {
  using namespace laplace::ui;

  class frame final : public widget {
  public:
    ~frame() final = default;

    void set_render_context(render::ptr_context con);

    void render() final;

  private:
    render::ptr_context m_render = render::context::get_default();
    graphics::flat::framebuffer m_buffer;
  };

  using ptr_frame = std::shared_ptr<frame>;
}

#endif
