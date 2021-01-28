#pragma once

#include "camera.h"
#include "instance.h"
#include "light.h"
#include <vector>

namespace laplace::render
{
    static constexpr real default_ambient_light = .1f;

    struct scene
    {
        real ambient_light = default_ambient_light;
        camera camera;
        vlight lights;
        vptr_instance objects;
    };

    using ptr_scene = std::shared_ptr<scene>;
}
