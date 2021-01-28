#include "buffer.h"

using namespace laplace;
using namespace ui;
using namespace text;
using namespace core;
using namespace graphics;
using namespace std;

void buffer::build(ref_painter paint)
{
    /*  TODO
     */
}

void buffer::build_monospace(ref_painter paint, int top, size_t char_width, size_t char_height)
{
    /*  TODO
     */
}

void buffer::build_monospace(cref_image img, int top, size_t char_width, size_t char_height)
{
    /*  TODO
     */
}

auto buffer::adjust(u8string_view text) -> renderer::area
{
    /*  TODO
     */

    return { 0, 0, 0 };
}

void buffer::render(int x, int y, u8string_view text)
{
    /*  TODO
     */
}
