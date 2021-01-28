#pragma once

#include "defs.h"
#include <memory>

namespace laplace::render
{
    struct camera
    {
        vec3 position = vec3(real(0));
        mat3 rotation = mat3(real(1));
    };

    using ref_camera = camera &;
    using cref_camera = const camera &;
    using ptr_camera = std::shared_ptr<camera>;
}
