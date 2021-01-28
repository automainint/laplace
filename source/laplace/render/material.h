#pragma once

#include "../core/element.h"
#include "../graphics/texture.h"
#include "defs.h"
#include "track.h"
#include <memory>

namespace laplace::render
{
    class material final : public core::element
    {
    public:
        graphics::ptr_texture colormap;
        graphics::ptr_texture normalmap;
        graphics::ptr_texture diffusemap;
        graphics::ptr_texture specularmap;
        graphics::ptr_texture reflectionmap;
        graphics::ptr_texture refractionmap;
        graphics::ptr_texture emissionmap;

        real_track diffuse_factor;
        real_track specular_factor;
        real_track reflection_factor;
        real_track refraction_ratio;
        real_track emission_factor;

        material();
        ~material();
    };

    using ptr_material = std::shared_ptr<material>;
}
