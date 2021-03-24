#pragma once

#include "../widget.h"

namespace laplace::ui::elem
{
    class slider : public widget
    {
    public:
        struct state
        {
            size_t level;
            rect slider_rect;
            int begin;
            int end;
            int value;
        };
    };
}
