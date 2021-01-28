#include "lobby.h"

using namespace quadwar_app;
using namespace std;

lobby::lobby()
{
    m_abort->set_text(c_abort);
    m_start->set_text(c_start);
}

lobby::~lobby()
{
    if (auto p = m_info->get_parent(); p)
    {
        p->detach(m_info);
        p->detach(m_slots_area);
        p->detach(m_abort);
        p->detach(m_start);
    }
}

void lobby::on_abort(lobby::event_abort ev)
{
    m_abort->on_click([=](ui::ptr_widget) { ev(); });
}

void lobby::on_start(lobby::event_start ev)
{
    m_start->on_click([=](ui::ptr_widget) { ev(); });
}

void lobby::attach_to(ui::ptr_widget w)
{
    if (w)
    {
        w->attach(m_info);
        w->attach(m_slots_area);
        w->attach(m_abort);
        w->attach(m_start);
    }
}

void lobby::adjust_layout(size_t width, size_t height)
{
    m_info->set_rect({
        static_cast<int>(side_size + spacing),
        static_cast<int>(height - line_height * 2 + spacing),
        width - spacing * 2 - side_size * 2,
        line_height - spacing * 2 - side_size * 2
    });

    m_slots_area->set_rect({
        static_cast<int>(side_size + spacing),
        static_cast<int>(side_size + spacing),
        width - spacing * 2 - side_size * 2,
        height - line_height * 2 - spacing * 2 - side_size * 2
    });

    m_abort->set_rect({
        static_cast<int>(spacing + side_size),
        static_cast<int>(height - side_size - line_height + spacing),
        button_width - spacing * 2,
        line_height - spacing * 2
    });

    m_start->set_rect({
        static_cast<int>(width - side_size - button_width + spacing),
        static_cast<int>(height - side_size - line_height + spacing),
        button_width - spacing * 2,
        line_height - spacing * 2
    });
}

void lobby::show_info(u8string_view text)
{
    m_info->set_text(text);
}

void lobby::set_slot(size_t index, size_t id_actor, u8string_view name)
{
    log("lobby: set slot %zd", index);

}

void lobby::remove_slot(size_t index)
{
    log("lobby: remove slot %zd", index);

}

void lobby::set_visible(bool is_visible)
{
    if (m_is_visible != is_visible)
    {
        m_is_visible = is_visible;
        m_slots_area->set_visible(is_visible);
        m_info->set_visible(is_visible);
        m_abort->set_visible(is_visible);
        m_start->set_visible(is_visible);
    }
}

void lobby::set_start_enabled(bool is_enabled)
{
    m_start->set_enabled(is_enabled);
}
