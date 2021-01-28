#pragma once

#include "../program.h"
#include "../defs.h"

namespace laplace::graphics::flat
{
    class sprite_shader
    {
    public:
        struct vertex
        {
            vec2 position;
            vec2 texcoord;
        };

        sprite_shader(ref_istream vert, ref_istream frag);
        ~sprite_shader();

        void use();

        void set_position(cref_vec2 position);
        void set_scale(cref_vec2 scale);
        void set_texture(size_t id);

    private:
        int n_position;
        int n_scale;
        int n_texture;

        program m_program;
    };

    using ptr_sprite_shader = std::shared_ptr<sprite_shader>;
}
