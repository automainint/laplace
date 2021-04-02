#pragma once

#include "../texture.h"
#include "mesh_shader.h"

namespace laplace::graphics::tridi
{
    class mesh_buffer
    {
    public:
        using vertex = mesh_shader::vertex;
        using vvertex = std::vector<vertex>;
        using cref_vvertex = const vvertex &;

        mesh_buffer();
        ~mesh_buffer();

        void setup(cref_vvertex vertices, span_cuint32 indices, bool is_static);
        void update(cref_vvertex vertices);

        void render();

    private:
        uint32_t m_id_array;
        uint32_t m_ids[2];
        size_t m_count;
    };

    using ptr_mesh_buffer = std::shared_ptr<mesh_buffer>;
}
