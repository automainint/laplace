#pragma once

#include "motion.h"
#include "../graphics/tridi/mesh_buffer.h"
#include <vector>
#include <memory>

namespace laplace::render
{
    class model;

    using ptr_model = std::shared_ptr<model>;

    class mesh
    {
    public:
        mesh();
        ~mesh();

        void setup(ptr_model source);
        void adjust(cref_motion state);

    private:
        ptr_model m_source;
    };

    using cref_mesh = const mesh &;
    using ptr_mesh = std::shared_ptr<mesh>;
    using vptr_mesh = std::vector<ptr_mesh>;
}
