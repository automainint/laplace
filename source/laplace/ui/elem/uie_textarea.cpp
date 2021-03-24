#include "textarea.h"

using namespace laplace;
using namespace ui;
using namespace elem;
using namespace text;
using namespace core;
using namespace std;

void textarea::render()
{
    textarea_render();

    up_to_date();
}

void textarea::set_renderer(text::ptr_renderer renderer)
{
    m_renderer = renderer;
    set_expired(true);
}

void textarea::set_text(u8string_view text)
{
    m_text = text;
    set_expired(true);
}

void textarea::set_line_height(int line_height)
{
    m_line_height = line_height;
    set_expired(true);
}

void textarea::textarea_render()
{
    size_t i0 = 0;
 
    const auto y1 = get_absolute_y() + get_rect().height;
    auto y = m_line_height > 0 ? get_absolute_y() : y1 - m_line_height;

    for (size_t i = 0; i <= m_text.length(); i++)
    {
        if (i == m_text.length() || m_text[i] == u8'\n')
        {
            if (i0 < i)
            {
                m_renderer->render(
                    get_absolute_x(), y,
                    { m_text.data() + i0, i - i0 }
                );
            }

            if (y < 0) break;

            y += m_line_height;

            if (y >= y1) break;

            i0 = i + 1;
        }
    }
}
