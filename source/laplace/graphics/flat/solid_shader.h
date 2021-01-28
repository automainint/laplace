#pragma once

#include "../program.h"
#include "../defs.h"

namespace laplace::graphics::flat
{
    class solid_shader
    {
    public:
        struct vertex
        {
            vec2 position;
            vec4 color;
        };

        solid_shader(ref_istream vert, ref_istream frag);
        ~solid_shader();

        void use();

        void set_position(cref_vec2 position);
        void set_scale(cref_vec2 scale);

    private:
        int n_position;
        int n_scale;

        program m_program;
    };

    using ptr_solid_shader = std::shared_ptr<solid_shader>;
}
