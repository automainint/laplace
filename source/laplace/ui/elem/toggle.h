#pragma once

#include "../widget.h"

namespace laplace::ui::elem
{
    class toggle : public widget
    {
    public:
        struct state
        {
            size_t level;
            rect toggle_rect;
            bool is_pressed;
            bool is_on;
        };
    };
}
