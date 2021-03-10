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

#include "../core/embedded.h"
#include "../core/utils.h"
#include "../graphics/utils.h"
#include "application.h"
#include "config.h"
#include <fstream>

namespace laplace::stem {
  using std::make_shared, std::make_unique, config::load,
      config::k_frame, config::k_caption, config::k_shaders,
      config::k_flat_solid, config::k_flat_sprite, config::k_vertex,
      config::k_fragment, config::k_folder, platform::window,
      platform::input, platform::glcontext, platform::ref_window,
      platform::ref_input, platform::ref_glcontext,
      graphics::flat::solid_shader, graphics::flat::sprite_shader,
      core::cref_family, std::wstring, std::wstring_view,
      std::unique_ptr, std::istream, std::ifstream;

  application::application(int argc, char **argv, cref_family def_cfg) {
    m_config = load(argc, argv, def_cfg);
  }

  application::~application() {
    config::save(m_config);
  }

  auto application::run() -> int {
    size_t frame_width  = m_config[k_frame][0];
    size_t frame_height = m_config[k_frame][1];
    size_t frame_rate   = m_config[k_frame][2];

    gl::require_extensions({ "GL_ARB_framebuffer_object" });

    m_window = make_shared<window>();
    m_gl     = make_shared<glcontext>(m_window);

    if (!gl::is_ok()) {
      error(__FUNCTION__, "Initialization failed.");
      return 0;
    }

    m_input = make_shared<input>();

    m_window->set_input(m_input);
    m_window->set_name(to_wstring(m_config[k_caption].get_string()));
    m_window->set_size(frame_width, frame_height);
    m_window->set_fullscreen_mode(
        frame_width, frame_height, frame_rate);

    m_window->on_init([this]() {
      init();
    });

    m_window->on_cleanup([this]() {
      cleanup();
    });

    m_window->on_size([this](size_t width, size_t height) {
      if (width > 0 && height > 0) {
        set_frame_size(width, height);
      }
    });

    m_window->on_frame([this](uint64_t delta_msec) {
      update(delta_msec);
      render();
    });

    return m_window->message_loop();
  }

  void application::init() {
    graphics::init();

    m_ui     = ui::context::get_default();
    m_render = render::context::get_default();

    load_shaders();

    m_window->set_visible(true);
  }

  void application::cleanup() {
    m_ui.reset();
    m_render.reset();
  }

  void application::update(uint64_t delta_msec) { }

  void application::render() {
    m_gl->swap_buffers();
  }

  void application::set_frame_size(size_t width, size_t height) {
    adjust_layout(static_cast<int>(width), static_cast<int>(height));

    graphics::viewport(0, 0, width, height);

    if (m_ui) {
      m_ui->set_frame_size(width, height);
    }
  }

  void application::adjust_layout(int width, int height) {
    adjust_frame_size(width, height);
  }

  auto application::get_window() -> ref_window {
    return *m_window;
  }

  auto application::get_gl() -> ref_glcontext {
    return *m_gl;
  }

  auto application::get_input() -> ref_input {
    return *m_input;
  }

  void application::load_shaders() {
    if (m_config.has(k_shaders)) {
      auto &s_cfg = m_config[k_shaders];

      if (s_cfg.has(k_flat_solid)) {
        const auto vert_path = shader_path(k_flat_solid, k_vertex);
        const auto frag_path = shader_path(k_flat_solid, k_fragment);
        const auto vert      = open(vert_path);
        const auto frag      = open(frag_path);

        m_flat_solid = make_shared<solid_shader>(*vert, *frag);
      }

      if (s_cfg.has(k_flat_sprite)) {
        const auto vert_path = shader_path(k_flat_sprite, k_vertex);
        const auto frag_path = shader_path(k_flat_sprite, k_fragment);
        const auto vert      = open(vert_path);
        const auto frag      = open(frag_path);

        m_flat_sprite = make_shared<sprite_shader>(*vert, *frag);
      }

      if (m_render) {
        m_render->setup(m_flat_solid);
        m_render->setup(m_flat_sprite);
      }
    }
  }

  void application::adjust_frame_size(int width, int height) {
    if (width != 0 && height != 0) {
      const auto x0 = width < 0 ? 1.f : -1.f;
      const auto y0 = height < 0 ? -1.f : 1.f;
      const auto w  = 2.f / static_cast<float>(width);
      const auto h  = -2.f / static_cast<float> (height);

      if (m_flat_solid) {
        m_flat_solid->use();
        m_flat_solid->set_position({ x0, y0 });
        m_flat_solid->set_scale({ w, h });
      }

      if (m_flat_sprite) {
        m_flat_sprite->use();
        m_flat_sprite->set_position({ x0, y0 });
        m_flat_sprite->set_scale({ w, h });
      }
    }
  }

  auto application::shader_path(const char *name,
                                const char *type) const -> wstring {
    return to_wstring(m_config[k_shaders][k_folder].get_string()) +
           to_wstring(m_config[k_shaders][name][type].get_string());
  }

  auto application::open(wstring_view file_name)
      -> unique_ptr<istream> {
    verb("Load: '%s'", to_string(file_name).c_str());

    if (embedded::scan(file_name)) {
      return make_unique<ibytestream>(embedded::open(file_name));
    }

    return make_unique<ifstream>(file_name.data());
  }
}
