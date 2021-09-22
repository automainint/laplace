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

#include "../core/embedded.h"
#include "../core/utils.h"
#include "../graphics/flat/solid_shader.h"
#include "../graphics/flat/sprite_shader.h"
#include "../graphics/utils.h"
#include "../ui/widget.h"
#include "config.h"
#include "ui/context.h"
#include <filesystem>
#include <fstream>

namespace laplace::stem {
  using std::make_shared, std::make_unique, config::load,
      config::k_frame, config::k_caption, config::k_shaders,
      config::k_flat_solid, config::k_flat_sprite, config::k_vertex,
      config::k_fragment, config::k_folder, platform::window,
      platform::input, platform::glcontext, platform::ref_window,
      core::cref_input_handler, platform::ref_glcontext,
      graphics::flat::solid_shader, graphics::flat::sprite_shader,
      core::cref_family, std::wstring, std::wstring_view,
      std::unique_ptr, std::istream, std::ifstream,
      std::filesystem::path;

  application::application(int         argc,
                           char **     argv,
                           cref_family def_cfg) {
    m_config = load(argc, argv, def_cfg);
  }

  application::~application() {
    config::save(m_config);
  }

  auto application::run() -> int {
    sl::whole frame_width  = m_config[k_frame][0];
    sl::whole frame_height = m_config[k_frame][1];
    sl::whole frame_rate   = m_config[k_frame][2];

    gl::require_extensions({ "GL_ARB_framebuffer_object" });

    m_window = make_shared<window>();
    m_gl     = make_shared<glcontext>(m_window);

    if (!gl::is_ok()) {
      error_("Initialization failed.", __FUNCTION__);
      return 0;
    }

    m_input = make_shared<input>();

    wrap_input();

    m_window->set_input(m_input);
    m_window->set_name(to_wstring(m_config[k_caption].get_string()));
    m_window->set_size(frame_width, frame_height);
    m_window->set_fullscreen_mode(frame_width, frame_height,
                                  frame_rate);

    m_window->on_init([this]() { init(); });

    m_window->on_cleanup([this]() { cleanup(); });

    m_window->on_size([this](sl::whole width, sl::whole height) {
      if (width > 0 && height > 0) {
        set_frame_size(width, height);
      }
    });

    m_window->on_frame([this](sl::time delta_msec) {
      update(delta_msec);
      render();
    });

    return m_window->message_loop();
  }

  void application::init() {
    graphics::init();

    m_render = render::context::get_default();

    load_shaders();
    setup_ui();

    m_window->set_visible(true);
  }

  void application::cleanup() {
    m_ui.reset();
    m_render.reset();
  }

  void application::update(sl::time delta_msec) { }

  void application::render() {
    m_gl->swap_buffers();
  }

  void application::set_frame_size(sl::whole width,
                                   sl::whole height) {
    adjust_layout(width, height);

    graphics::viewport(0, 0, width, height);

    if (m_ui) {
      m_ui->frame_width  = width;
      m_ui->frame_height = height;
    }
  }

  void application::adjust_layout(sl::whole width, sl::whole height) {
    adjust_frame_size(width, height);
  }

  auto application::get_window() -> ref_window {
    return *m_window;
  }

  auto application::get_gl() -> ref_glcontext {
    return *m_gl;
  }

  auto application::get_input() -> cref_input_handler {
    return m_input_handler;
  }

  void application::setup_ui() {
    m_ui = make_shared<ui::context_impl>();
    ui::widget::set_default_context(m_ui);
  }

  void application::wrap_input() {
    m_input_handler.is_capslock = [&]() {
      return m_input->is_capslock();
    };

    m_input_handler.is_numlock = [&]() {
      return m_input->is_numlock();
    };

    m_input_handler.is_scrolllock = [&]() {
      return m_input->is_scrolllock();
    };

    m_input_handler.is_alt = [&]() { return m_input->is_alt(); };

    m_input_handler.is_shift = [&]() { return m_input->is_shift(); };

    m_input_handler.is_control = [&]() {
      return m_input->is_control();
    };

    m_input_handler.is_key_down = [&](sl::index key) {
      return m_input->is_key_down(key);
    };

    m_input_handler.is_key_up = [&](sl::index key) {
      return m_input->is_key_up(key);
    };

    m_input_handler.is_key_changed = [&](sl::index key) {
      return m_input->is_key_changed(key);
    };

    m_input_handler.is_key_pressed = [&](sl::index key) {
      return m_input->is_key_pressed(key);
    };

    m_input_handler.is_key_unpressed = [&](sl::index key) {
      return m_input->is_key_unpressed(key);
    };

    m_input_handler.get_mouse_resolution_x = [&]() {
      return m_input->get_mouse_resolution_x();
    };

    m_input_handler.get_mouse_resolution_y = [&]() {
      return m_input->get_mouse_resolution_y();
    };

    m_input_handler.get_mouse_x = [&]() {
      return m_input->get_mouse_x();
    };

    m_input_handler.get_mouse_y = [&]() {
      return m_input->get_mouse_y();
    };

    m_input_handler.get_mouse_delta_x = [&]() {
      return m_input->get_mouse_delta_x();
    };

    m_input_handler.get_mouse_delta_y = [&]() {
      return m_input->get_mouse_delta_y();
    };

    m_input_handler.get_cursor_x = [&]() {
      return m_input->get_cursor_x();
    };

    m_input_handler.get_cursor_y = [&]() {
      return m_input->get_cursor_y();
    };

    m_input_handler.get_wheel_delta = [&]() {
      return m_input->get_wheel_delta();
    };

    m_input_handler.get_text = [&]() { return m_input->get_text(); };
  }

  void application::load_shaders() {
    if (m_config.has(k_shaders) && m_render) {
      auto &s_cfg = m_config[k_shaders];

      if (s_cfg.has(k_flat_solid)) {
        const auto vert_path = shader_path(k_flat_solid, k_vertex);
        const auto frag_path = shader_path(k_flat_solid, k_fragment);
        const auto vert      = open(vert_path);
        const auto frag      = open(frag_path);

        m_render->setup(make_shared<solid_shader>(*vert, *frag));
      }

      if (s_cfg.has(k_flat_sprite)) {
        const auto vert_path = shader_path(k_flat_sprite, k_vertex);
        const auto frag_path = shader_path(k_flat_sprite, k_fragment);
        const auto vert      = open(vert_path);
        const auto frag      = open(frag_path);

        m_render->setup(make_shared<sprite_shader>(*vert, *frag));
      }
    }
  }

  void application::adjust_frame_size(sl::whole width,
                                      sl::whole height) {
    if (width != 0 && height != 0) {
      if (m_render)
        m_render->adjust_frame_size(width, height);
    }
  }

  auto application::shader_path(const char *name,
                                const char *type) const -> wstring {
    return to_wstring(m_config[k_shaders][k_folder].get_string()) +
           to_wstring(m_config[k_shaders][name][type].get_string());
  }

  auto application::open(wstring_view file_name)
      -> unique_ptr<istream> {
    verb(fmt("Load: '%s'", to_string(file_name).c_str()));

    if (embedded::scan(file_name)) {
      return make_unique<ibytestream>(embedded::open(file_name));
    }

    return make_unique<ifstream>(path(file_name));
  }
}
