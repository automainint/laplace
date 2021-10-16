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

#include "app_flat.h"

#include "../graphics/utils.h"

namespace laplace::stem {
  using std::make_shared, std::abs, core::unival, ui::frame;

  app_flat::app_flat(int           argc,
                     char **       argv,
                     unival const &def_cfg) noexcept :
      application(argc, argv, def_cfg) { }

  void app_flat::init() noexcept {
    application::init();

    m_ui = make_shared<frame>();
  }

  void app_flat::cleanup() noexcept {
    m_ui.reset();

    application::cleanup();
  }

  void app_flat::update(sl::time delta_msec) noexcept {
    m_ui->tick(delta_msec, get_input(), false);
  }

  void app_flat::render() noexcept {
    graphics::clear(clear_color);
    m_ui->render(get_ui_context());
    finish_and_swap();
  }

  void app_flat::adjust_layout(sl::whole width,
                               sl::whole height) noexcept {
    application::adjust_layout(width, height);

    m_ui->set_rect(
        { .x = 0, .y = 0, .width = width, .height = height });

    m_ui->refresh();
  }
}
