#pragma once

#include "mesh.h"

namespace laplace::render
{
    struct instance
    {
        vec3 position;
        quat rotation;
        ptr_mesh object;
    };

    using ptr_instance = std::shared_ptr<instance>;
    using vptr_instance = std::vector<ptr_instance>;
}
