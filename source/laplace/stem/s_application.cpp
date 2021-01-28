/*  laplace/stem/application.cpp
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

#include "application.h"
#include "config.h"
#include "../core/utils.h"
#include "../core/embedded.h"
#include "../graphics/utils.h"
#include <fstream>

using namespace laplace;
using namespace stem;
using namespace config;
using namespace core;
using namespace graphics;
using namespace platform;
using namespace std;

application::application(int argc, char **argv, core::cref_family def_cfg)
{
    m_config = config::load(argc, argv, def_cfg);
}

application::~application()
{
    config::save(m_config);
}

auto application::run() -> int
{
    size_t frame_width  = m_config[k_frame][0];
    size_t frame_height = m_config[k_frame][1];
    size_t frame_rate   = m_config[k_frame][2];

    gl::require_extensions({
        "GL_ARB_framebuffer_object"
    });

    m_window    = make_shared<window>();
    m_gl        = make_shared<glcontext>(m_window);

    if (!gl::is_ok())
    {
        cerr << __FUNCTION__ << ": Initialization failed." << endl;
        return 0;
    }

    m_input     = make_shared<input>();

    m_window->set_input(m_input);
    m_window->set_name(to_wstring(m_config[k_caption].get_string()));
    m_window->set_size(frame_width, frame_height);
    m_window->set_fullscreen_mode(frame_width, frame_height, frame_rate);

    m_window->on_init([this]() {
        init();
    });

    m_window->on_cleanup([this]() {
        cleanup();
    });

    m_window->on_size([this](size_t width, size_t height) {
        if (width > 0 && height > 0)
        {
            set_frame_size(width, height);
        }
    });

    m_window->on_frame([this](size_t delta_msec) {
        update(delta_msec);
        render();
    });

    return m_window->message_loop();
}

void application::init()
{
    graphics::init();

    m_ui        = ui::context::get_default();
    m_render    = render::context::get_default();

    load_shaders();

    m_window->set_visible(true);
}

void application::cleanup()
{
    m_ui.reset();
    m_render.reset();
}

void application::update(size_t delta_msec) { }

void application::render()
{
    m_gl->swap_buffers();
}

void application::set_frame_size(size_t width, size_t height)
{
    graphics::viewport(0, 0, width, height);

    adjust_frame_size(width, height);

    if (m_ui)
    {
        m_ui->set_frame_size(width, height);
    }
}

auto application::get_window() -> ref_window
{
    return *m_window;
}

auto application::get_gl() -> ref_glcontext
{
    return *m_gl;
}

auto application::get_input() -> ref_input
{
    return *m_input;
}

void application::load_shaders()
{
    if (m_config.has(k_shaders))
    {
        auto &s_cfg         = m_config[k_shaders];
        bool setup_flat     = false;
        bool setup_tridi    = false;

        if (s_cfg.has(k_flat_solid))
        {
            setup_flat      = true;
            auto vert_path  = shader_path(k_flat_solid, k_vertex);
            auto frag_path  = shader_path(k_flat_solid, k_fragment);
            auto vert       = open(vert_path);
            auto frag       = open(frag_path);
            m_flat_solid    = make_shared<flat::solid_shader>(*vert, *frag);
        }

        if (s_cfg.has(k_flat_sprite))
        {
            setup_flat      = true;
            auto vert_path  = shader_path(k_flat_sprite, k_vertex);
            auto frag_path  = shader_path(k_flat_sprite, k_fragment);
            auto vert       = open(vert_path);
            auto frag       = open(frag_path);
            m_flat_sprite   = make_shared<flat::sprite_shader>(*vert, *frag);
        }

        if (setup_flat)
        {
            setup_to(m_ui);
        }

        if (setup_tridi)
        {
            setup_to(m_render);
        }
    }
}

void application::adjust_frame_size(size_t width, size_t height)
{
    if (m_flat_solid)
    {
        m_flat_solid->use();
        m_flat_solid->set_position({ -1.f, 1.f });
        m_flat_solid->set_scale({ 2.f / width, -2.f / height });
    }

    if (m_flat_sprite)
    {
        m_flat_sprite->use();
        m_flat_sprite->set_position({ -1.f, 1.f });
        m_flat_sprite->set_scale({ 2.f / width, -2.f / height });
    }
}

void application::setup_to(ui::ptr_context cont)
{
    if (cont)
    {
        cont->setup(m_flat_solid);
        cont->setup(m_flat_sprite);
    }
}

void application::setup_to(render::ptr_context cont)
{
}

auto application::shader_path(const char *name, const char *type) const -> wstring
{
    return
        to_wstring(m_config[k_shaders][k_folder].get_string()) +
        to_wstring(m_config[k_shaders][name][type].get_string());
}

auto application::open(wstring_view file_name) -> unique_ptr<istream>
{
    wcout << "Load: " << file_name << endl;

    if (embedded::scan(file_name))
    {
        return make_unique<ibytestream>(embedded::open(file_name));
    }

    return make_unique<ifstream>(file_name.data());
}
