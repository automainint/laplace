#pragma once

#include "defs.h"

namespace laplace::graphics
{
    void init();
    void viewport(int x, int y, size_t width, size_t height);
    void clear(cref_vec4 color);
    void clear_color_buffer(cref_vec4 color);

    void prepare_ui();
    void prepare_render();

    void set_blend_enabled(bool is_enabled);
}
