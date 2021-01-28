#include "panel.h"
#include "../context.h"

using namespace laplace;
using namespace ui;
using namespace elem;

void panel::set_context(ptr_context cont)
{
    m_context = cont ? cont : ui::context::get_default();
    set_expired(true);
}

void panel::render()
{
    assert(m_context);
    m_context->render(get_state());

    draw_childs();

    up_to_date();
}

auto panel::get_state() const -> panel::state
{
    return {
        get_level(),
        get_absolute_rect(),
        is_enabled()
    };
}
