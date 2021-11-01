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

#include "../core/input_handler.h"
#include "../core/unival.h"
#include "../graphics/defs.h"
#include "../platform/wrap.h"
#include "../render/context.h"
#include "../ui/context.h"
#include <chrono>

namespace laplace::stem {
  class application {
  public:
    application(int                 argc,
                char              **argv,
                core::unival const &def_cfg) noexcept;

    virtual ~application() noexcept;

    auto run() noexcept -> int;

  protected:
    core::unival m_config;

    virtual void init() noexcept;
    virtual void cleanup() noexcept;
    virtual void update(sl::time delta_msec) noexcept;
    virtual void render() noexcept;
    virtual void set_frame_size(sl::whole width,
                                sl::whole height) noexcept;
    virtual void adjust_layout(sl::whole width,
                               sl::whole height) noexcept;

    void lock_fps(sl::whole fps) noexcept;
    void finish_and_swap() noexcept;

    [[nodiscard]] auto get_window() noexcept -> platform::window &;
    [[nodiscard]] auto get_gl() noexcept -> platform::glcontext &;
    [[nodiscard]] auto get_input() noexcept
        -> core::cref_input_handler;
    [[nodiscard]] auto get_ui_context() noexcept -> ui::context &;

  private:
    void load_shaders() noexcept;
    void wrap_input() noexcept;
    void adjust_frame_size(sl::whole width,
                           sl::whole height) noexcept;

    [[nodiscard]] auto shader_path(const char *name,
                                   const char *type) const noexcept
        -> std::wstring;

    [[nodiscard]] static auto open(
        std::wstring_view file_name) noexcept
        -> std::unique_ptr<std::istream>;

    std::shared_ptr<platform::window>    m_window;
    std::shared_ptr<platform::glcontext> m_gl;
    std::shared_ptr<platform::input>     m_input;

    render::ptr_context m_render;
    ui::ptr_context     m_ui;
    core::input_handler m_input_handler;

    std::chrono::steady_clock::time_point m_frame_time;
  };
}

#endif
