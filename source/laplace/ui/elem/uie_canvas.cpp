#include "canvas.h"
#include "../context.h"

using namespace laplace;
using namespace ui;
using namespace elem;
using namespace graphics;

canvas::canvas()
{
    m_context = context::get_default();
}

void canvas::set_context(ptr_context cont)
{
    m_context = cont ? cont : context::get_default();
    set_expired(true);
}

void canvas::render()
{
    m_texture.image_2d_nearest(m_image);
    m_context->render(get_rect(), m_texture);
}

auto canvas::image() -> ref_image
{
    return m_image;
}

auto canvas::get_image() const -> cref_image
{
    return m_image;
}
