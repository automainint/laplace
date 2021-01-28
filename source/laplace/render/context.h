#pragma once

#include "projection.h"
#include "scene.h"
#include "../graphics/tridi/mesh_shader.h"
#include "../graphics/tridi/uvmap_shader.h"
#include "../graphics/tridi/shadow_shader.h"
#include "../graphics/tridi/reflect_shader.h"
#include "../graphics/tridi/refract_shader.h"
#include <memory>
#include <array>
#include <string>

namespace laplace::render
{
    enum class mode : uint8_t
    {
        disabled,
        low,
        high,
        ultra
    };

    struct settings
    {
        mode blending;
        mode lighting;
        mode shadows;
        mode volumes;
        mode reflection;
        mode refraction;
    };

    using cref_settings = const settings &;

    class context
    {
    public:
        using ptr_context = std::shared_ptr<context>;

        static constexpr settings default_settings = {
            mode::disabled,
            mode::disabled,
            mode::disabled,
            mode::disabled,
            mode::disabled,
            mode::disabled
        };

        void setup(graphics::tridi::ptr_mesh_shader shader);
        void setup(graphics::tridi::ptr_uvmap_shader shader);
        void setup(graphics::tridi::ptr_shadow_shader shader);
        void setup(graphics::tridi::ptr_reflect_shader shader);
        void setup(graphics::tridi::ptr_refract_shader shader);

        void set_settings(cref_settings sets);
        void set_projection(cref_projection proj);
        void set_scene(ptr_scene sce);

        void render();

        static auto get_default() -> ptr_context;

    private:
        static std::weak_ptr<context> m_default;

        settings    m_settings = default_settings;
        projection  m_projection;
        ptr_scene   m_scene;

        graphics::tridi::ptr_mesh_shader    m_mesh_shader;
        graphics::tridi::ptr_uvmap_shader   m_uvmap_shader;
        graphics::tridi::ptr_shadow_shader  m_shadow_shader;
        graphics::tridi::ptr_reflect_shader m_reflect_shader;
        graphics::tridi::ptr_refract_shader m_refract_shader;
    };

    using ref_context = context &;
    using cref_context = const context &;
    using ptr_context = context::ptr_context;
}
