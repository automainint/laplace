#include "frame.h"
#include "../graphics/utils.h"
#include <iostream>

using namespace laplace;
using namespace ui;
using namespace graphics;
using namespace flat;
using namespace std;

frame::frame()
{
    m_context = context::get_default();
}

void frame::set_context(ptr_context cont)
{
    m_context = cont;
}

void frame::render()
{
    if (m_context)
    {
        rect r = {
            0, 0,
            m_context->get_frame_width(),
            m_context->get_frame_height()
        };

        if (is_widget_changed())
        {
            m_buffer.set_size(r.width, r.height);
        }

        if (is_widget_changed() || has_childs_expired())
        {
            m_buffer.render([=]() {
                graphics::clear_color_buffer({ 0.f, 0.f, 0.f, 0.f });

                widget_render();
            });

            graphics::viewport(0, 0, r.width, r.height);
        }

        m_context->render(r, m_buffer.color_texture);

        up_to_date();
    }
}
