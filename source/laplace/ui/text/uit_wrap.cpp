#include "wrap.h"
#include <cassert>

using namespace laplace;
using namespace ui;
using namespace core;
using namespace text;
using namespace graphics;
using namespace std;

wrap::wrap(ptr_painter paint, ptr_context cont) {
  m_context = cont;
  m_paint   = paint;
}

void wrap::set_context(ptr_context cont) {
  m_context = cont;
}

auto wrap::adjust(u8string_view text) -> area {
  return m_paint ? m_paint->adjust(text) : area {};
}

void wrap::render(int x, int y, u8string_view text) {
  if (m_paint && m_tex) {
    image img;

    auto a = m_paint->adjust(text);

    img.set_size(                     //
        static_cast<size_t>(a.width), //
        static_cast<size_t>(a.height));

    m_paint->print(img, 0, a.top, text);
    m_tex->image_2d_nearest(img);

    if (!m_context) {
      m_context = context::get_default();
    }

    m_context->render(rect { .x      = x,
                             .y      = y,
                             .width  = static_cast<int>(a.width),
                             .height = static_cast<int>(a.height) },
                      *m_tex);
  }
}
