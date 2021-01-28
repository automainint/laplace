#pragma once

#include "mesh_shader.h"

namespace laplace::graphics::tridi
{
    class refract_shader
    {
    public:
        using vertex = mesh_shader::vertex;

        using cref_view = mesh_shader::cref_view;
        using cref_material = mesh_shader::cref_material;

        refract_shader(ref_istream vert, ref_istream frag);
        ~refract_shader();

        void use();

        void set_view(cref_view view);
        void set_material(cref_material mat);

    private:
        int n_full_transform;
        int n_material_colormap;

        program m_program;
    };

    using ptr_refract_shader = std::shared_ptr<refract_shader>;
}
