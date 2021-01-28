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

#pragma once

#include "../core/family.h"
#include "../platform/wrap.h"
#include "../graphics/defs.h"
#include "../graphics/flat/solid_shader.h"
#include "../graphics/flat/sprite_shader.h"
#include "../ui/context.h"
#include "../render/context.h"

namespace laplace::stem
{
    class application
    {
    public:
        application(int argc, char **argv, core::cref_family def_cfg);
        virtual ~application();

        auto run() -> int;

    protected:
        graphics::flat::ptr_solid_shader    m_flat_solid;
        graphics::flat::ptr_sprite_shader   m_flat_sprite;

        core::family m_config;

        virtual void init();
        virtual void cleanup();
        virtual void update(size_t delta_msec);
        virtual void render();
        virtual void set_frame_size(size_t width, size_t height);

        auto get_window()   -> platform::ref_window;
        auto get_gl()       -> platform::ref_glcontext;
        auto get_input()    -> platform::ref_input;

    private:
        void load_shaders();
        void adjust_frame_size(size_t width, size_t height);

        void setup_to(ui::ptr_context cont);
        void setup_to(render::ptr_context cont);

        auto shader_path(const char *name, const char *type) const->std::wstring;
        auto open(std::wstring_view file_name)->std::unique_ptr<std::istream>;

        platform::ptr_window    m_window;
        platform::ptr_glcontext m_gl;
        platform::ptr_input     m_input;

        ui::ptr_context         m_ui;
        render::ptr_context     m_render;
    };
}
