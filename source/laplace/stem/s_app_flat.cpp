/*  laplace/stem/app_flat.cpp
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

using namespace laplace;
using namespace core;
using namespace stem;
using namespace ui;
using namespace std;

app_flat::app_flat(int argc, char **argv, cref_family def_cfg) : application(argc, argv, def_cfg) { }
app_flat::~app_flat() { }

void app_flat::init()
{
    application::init();

    m_ui = make_shared<frame>();
}

void app_flat::cleanup()
{
    m_ui.reset();

    application::cleanup();
}

void app_flat::update(size_t delta_msec)
{
    m_ui->tick(delta_msec, get_input());
}

void app_flat::render()
{
    graphics::clear(clear_color);

    m_ui->render();

    get_gl().swap_buffers();
}

void app_flat::set_frame_size(size_t width, size_t height)
{
    application::set_frame_size(width, height);

    m_ui->set_rect({ 0, 0, width, height });
    m_ui->refresh();
}
