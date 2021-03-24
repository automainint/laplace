#include "textbutton.h"
#include "../context.h"
#include <cassert>

using namespace laplace;
using namespace ui;
using namespace elem;
using namespace std;

void textbutton::render()
{
    assert(m_context);

    m_context->render(get_state(), m_text);

    up_to_date();
}

void textbutton::set_text(u8string_view text)
{
    m_text = text;
}
