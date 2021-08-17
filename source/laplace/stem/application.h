/*  laplace/stem/application.h
 *
 *      Base class for application.
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

#ifndef laplace_stem_application_h
#define laplace_stem_application_h

#include "../core/family.h"
#include "../graphics/defs.h"
#include "../platform/wrap.h"
#include "../render/context.h"
#include "../ui/context.h"

namespace laplace::stem {
  class application {
  public:
    application(int argc, char **argv, core::cref_family def_cfg);

    virtual ~application();

    auto run() -> int;

  protected:
    core::family m_config;

    virtual void init();
    virtual void cleanup();
    virtual void update(uint64_t delta_msec);
    virtual void render();
    virtual void set_frame_size(sl::whole width, sl::whole height);
    virtual void adjust_layout(sl::whole width, sl::whole height);

    [[nodiscard]] auto get_window() -> platform::ref_window;
    [[nodiscard]] auto get_gl() -> platform::ref_glcontext;
    [[nodiscard]] auto get_input() -> core::cref_input_handler;

  private:
    void wrap_input();
    void load_shaders();
    void adjust_frame_size(sl::whole width, sl::whole height);

    [[nodiscard]] auto shader_path(const char *name,
                                   const char *type) const
        -> std::wstring;

    [[nodiscard]] auto open(std::wstring_view file_name)
        -> std::unique_ptr<std::istream>;

    platform::ptr_window    m_window;
    platform::ptr_glcontext m_gl;
    platform::ptr_input     m_input;

    ui::ptr_context     m_ui;
    render::ptr_context m_render;

    core::input_handler m_input_handler;
  };
}

#endif
