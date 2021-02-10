/*  laplace/stem/s_app_flat.cpp
 *
 *      Base class for UI and 2D graphics application.
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

#include "../graphics/utils.h"
#include "app_flat.h"

namespace laplace::stem {
  using std::make_shared, std::abs, core::cref_family, ui::frame;

  app_flat::app_flat(int argc, char **argv, cref_family def_cfg) :
      application(argc, argv, def_cfg) { }

  app_flat::~app_flat() { }

  void app_flat::init() {
    application::init();

    m_ui = make_shared<frame>();
  }

  void app_flat::cleanup() {
    m_ui.reset();

    application::cleanup();
  }

  void app_flat::update(size_t delta_msec) {
    m_ui->tick(delta_msec, get_input(), false);
  }

  void app_flat::render() {
    graphics::clear(clear_color);

    m_ui->render();

    get_gl().swap_buffers();
  }

  void app_flat::adjust_layout(int width, int height) {
    application::adjust_layout(width, height);

    m_ui->set_rect({ .x      = 0,     //
                     .y      = 0,     //
                     .width  = abs(width), //
                     .height = abs(height) });

    m_ui->refresh();
  }
}
