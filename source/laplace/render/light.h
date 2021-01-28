#pragma once

#include "../graphics/texture.h"
#include "defs.h"

namespace laplace::render
{
    struct light
    {
        vec3 color;
        vec3 position;
        vec3 forward;
        vec3 up;
        real fov_angle;
        real intensity;
        real diffuse;
        real specular;
        real focus;
        graphics::ptr_texture lightmap;
    };

    using vlight = std::vector<light>;
    using cref_light = const light &;
}
