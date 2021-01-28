#pragma once

#include "uvmap_shader.h"

namespace laplace::graphics::tridi
{
    /*  UV depth map.
     */
    class shadow_shader
    {
    public:
        using vertex = mesh_shader::vertex;

        using cref_object = mesh_shader::cref_object;
        using cref_view = uvmap_shader::cref_view;
        using cref_material = mesh_shader::cref_material;
        using cref_light = mesh_shader::cref_light;

        shadow_shader(ref_istream vert, ref_istream frag);
        ~shadow_shader();

        void use();

    private:
        program m_program;
    };

    using ptr_shadow_shader = std::shared_ptr<shadow_shader>;
}

