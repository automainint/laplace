#include "painter.h"
#include "lcd.h"
#include <iostream>

using namespace laplace;
using namespace ui;
using namespace text;
using namespace std;

weak_ptr<painter> painter::m_default;

auto painter::adjust(u8string_view text) -> painter::area
{
    return { 0, 0, 0 };
}

void painter::print(graphics::ref_image img, int x, int y, u8string_view text) { }

auto painter::get_default() -> ptr_painter
{
    auto p = m_default.lock();

    if (!p)
    {
        verb("Init default LCD font.");

        auto lcd_font = make_shared<lcd>();

        lcd_font->set_size(
            default_lcd_char_top,
            default_lcd_char_width,
            default_lcd_char_height
        );

        lcd_font->set_bits(default_lcd_bits);

        p = lcd_font;
        m_default = p;
    }

    return p;
}
