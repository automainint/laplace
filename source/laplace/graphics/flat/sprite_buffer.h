#pragma once

#include "sprite_shader.h"

namespace laplace::graphics::flat
{
    class sprite_buffer
    {
    public:
        using vertex = sprite_shader::vertex;

        sprite_buffer();
        ~sprite_buffer();

        void render(float x, float y, float width, float height);

    private:
        uint32_t m_id;
    };
}
