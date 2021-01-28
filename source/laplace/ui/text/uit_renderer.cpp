#include "renderer.h"
#include "wrap.h"

using namespace laplace;
using namespace ui;
using namespace text;
using namespace std;

weak_ptr<renderer> renderer::m_default;

auto renderer::adjust(u8string_view text) -> renderer::area
{
    return { 0, 0, 0 };
}

void renderer::render(int x, int y, u8string_view text) { }

auto renderer::get_default() -> ptr_renderer
{
    auto p = m_default.lock();

    if (!p)
    {
        p = make_shared<wrap>(
            painter::get_default(),
            ptr_context() /* late init */
        );

        m_default = p;
    }

    return p;
}
