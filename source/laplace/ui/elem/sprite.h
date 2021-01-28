#pragma once

#include "panel.h"
#include "../../graphics/texture.h"

namespace laplace::ui::elem
{
    class sprite : public panel
    {
    public:
        struct state : panel::state
        {
            graphics::ptr_texture texture;
        };
    };
}
