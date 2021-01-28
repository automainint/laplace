#pragma once

#include "solid_shader.h"

namespace laplace::graphics::flat
{
    class solid_buffer
    {
    public:
        using vertex = solid_shader::vertex;

        solid_buffer();
        ~solid_buffer();

        void render(float x, float y, float width, float height, cref_vec4 color);

    private:
        uint32_t m_id;
    };
}
