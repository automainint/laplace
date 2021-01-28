#include "textedit.h"
#include "../context.h"
#include "../../core/utils.h"
#include "../../platform/keys.h"
#include <cassert>

using namespace laplace;
using namespace ui;
using namespace elem;
using namespace platform;
using namespace std;

void textedit::setup_filter(filter f)
{
    if (f)
    {
        m_filter = f;
    }
    else
    {
        m_filter = [](char32_t) -> bool {
            return true;
        };
    }
}

auto textedit::tick(size_t delta_msec, platform::ref_input in) -> bool
{
    return textedit_tick(in);
}

void textedit::render()
{
    assert(m_context);
    m_context->render(get_state());

    up_to_date();
}

void textedit::set_text(u8string_view text)
{
    if (m_text != text)
    {
        m_text = text;
        set_expired(true);
    }
}

void textedit::set_length_limit(size_t limit)
{
    m_length_limit = limit;

    if (m_text.size() > limit)
    {
        m_text.resize(limit);
        set_expired(true);
    }
}

void textedit::set_cursor(size_t cursor)
{
    if (m_cursor != cursor)
    {
        m_cursor = cursor;
        set_expired(true);
    }
}

void textedit::set_selection(int selection)
{
    if (m_selection != selection)
    {
        m_selection = selection;
        set_expired(true);
    }
}

auto textedit::get_text() const -> u8string_view
{
    return m_text;
}

auto textedit::get_cursor() const -> size_t
{
    return m_cursor;
}

auto textedit::get_selection() const -> int
{
    return m_selection;
}

auto textedit::get_state() const -> textedit::state
{
    return {
        panel::get_state(),
        has_focus(),
        u8string(get_text()),
        m_length_limit,
        get_cursor(),
        get_selection()
    };
}

auto textedit::update(
    ptr_widget object,
    textedit::state textedit_state,
    textedit::filter f,
    platform::ref_input in
) -> textedit::update_result
{
    auto event_status   = false;
    auto has_focus      = textedit_state.has_focus;

    auto x = in.get_cursor_x();
    auto y = in.get_cursor_y();

    auto has_cursor =
        contains(textedit_state.rect, x, y) &&
        (!object || object->event_allowed(x, y));

    auto text       = textedit_state.text;
    auto cursor     = textedit_state.cursor;
    auto selection  = textedit_state.selection;

    if (has_focus)
    {
        auto s = in.get_text();

        if (!s.empty())
        {
            for (auto c : s)
            {
                if (c == ctrl_backspace)
                {
                    size_t n = 0;
                    char32_t buf;

                    for (size_t i = 0; i < cursor;)
                    {
                        n = i;

                        if (!utf8_decode(text, i, buf))
                            break;
                    }

                    text.erase(text.begin() + n, text.begin() + cursor);

                    cursor = n;
                }
                else if (c != ctrl_delete)
                {
                    if (!f || f(c))
                    {
                        if (text.size() < textedit_state.length_limit)
                        utf8_encode(c, text, cursor);
                    }
                }
            }
        }
    }
    else if (has_cursor && in.is_key_pressed(key_lbutton))
    {
        event_status    = true;
        has_focus       = true;
    }

    return { event_status, has_focus, text, cursor, selection };
}

auto textedit::textedit_tick(platform::ref_input in) -> bool
{
    auto s = update(shared_from_this(), get_state(), m_filter, in);

    set_focus(s.has_focus);
    set_text(s.text);
    set_cursor(s.cursor);
    set_selection(s.selection);

    return s.event_status;
}
