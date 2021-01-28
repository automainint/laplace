#include "button.h"
#include "../context.h"
#include "../../platform/keys.h"
#include <cassert>

using namespace laplace;
using namespace ui;
using namespace elem;
using namespace platform;
using namespace std;

button::~button() { }

void button::on_click(event_button_click ev)
{
    m_on_click = ev;
}

auto button::tick(size_t delta_msec, ref_input in) -> bool
{
    return button_tick(in);
}

void button::render()
{
    assert(m_context);
    m_context->render(get_state());

    up_to_date();
}

void button::set_pressed(bool is_pressed)
{
    if (m_is_pressed != is_pressed)
    {
        m_is_pressed = is_pressed;
        set_expired(true);
    }
}

void button::set_cursor(bool has_cursor)
{
    if (m_has_cursor != has_cursor)
    {
        m_has_cursor = has_cursor;
        set_expired(true);
    }
}

auto button::is_pressed() const -> bool
{
    return m_is_pressed;
}

auto button::has_cursor() const -> bool
{
    return m_has_cursor;
}

auto button::get_state() const -> button::state
{
    return {
        panel::get_state(),
        m_is_pressed,
        m_has_cursor
    };
}

auto button::update(
    ptr_widget object,
    button::state button_state,
    event_button_click on_button_click,
    ref_input in
) -> update_result
{
    auto event_status   = false;
    auto is_pressed     = button_state.is_pressed;

    auto x = in.get_cursor_x();
    auto y = in.get_cursor_y();

    auto has_cursor = 
        contains(button_state.rect, x, y) &&
        (!object || object->event_allowed(x, y));

    if (is_pressed)
    {
        if (in.is_key_up(key_lbutton))
        {
            if (has_cursor && on_button_click)
            {
                on_button_click(object);
            }

            event_status = true;
            is_pressed = false;
        }
    }
    else if (in.is_key_down(key_lbutton))
    {
        if (has_cursor)
        {
            if (in.is_key_changed(key_lbutton))
            {
                event_status = true;
                is_pressed = true;
            }
            else
            {
                /*  Ignore cursor if left mouse button was
                 *  pressed outside of the UI element.
                 */

                has_cursor = false;
            }
        }
    }

    return { event_status, is_pressed, has_cursor };
}

auto button::button_tick(ref_input in) -> bool
{
    auto status = update(
        shared_from_this(),
        get_state(),
        m_on_click,
        in
    );

    set_pressed(status.is_pressed);
    set_cursor(status.has_cursor);

    return status.event_status;
}
