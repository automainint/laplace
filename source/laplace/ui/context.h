#pragma once

#include "elem/panel.h"
#include "elem/button.h"
#include "elem/textedit.h"
#include "text/renderer.h"
#include "../graphics/texture.h"
#include "../graphics/flat/solid_buffer.h"
#include "../graphics/flat/solid_shader.h"
#include "../graphics/flat/sprite_buffer.h"
#include "../graphics/flat/sprite_shader.h"

namespace laplace::ui
{
    class context
    {
    public:
        static constexpr graphics::vec4 default_colors[ ] = {
            { .15f,  .1f,  .2f, 1.f },
            { .25f,  .2f,  .3f, 1.f },
            { .45f,  .4f,  .5f, 1.f },
            { .85f,  .8f,  .9f, 1.f },
            { .15f, .15f, .15f, 1.f },
            { .25f, .25f, .25f, 1.f },
            { .45f, .45f, .45f, 1.f },
            { .85f, .85f, .85f, 1.f },
        };

        context(text::ptr_renderer font = text::renderer::get_default());

        void set_frame_size(size_t width, size_t height);
        void set_colors(const graphics::vec4 colors[8]);
        void set_font(text::ptr_renderer font);

        void setup(graphics::flat::ptr_solid_shader shader);
        void setup(graphics::flat::ptr_sprite_shader shader);

        void render(cref_rect r, graphics::ref_texture tex);

        void render(elem::panel::state panel_state);
        void render(elem::button::state button_state);
        void render(elem::button::state button_state, std::u8string_view text);
        void render(elem::textedit::state textedit_state);

        auto get_frame_width() const -> size_t;
        auto get_frame_height() const -> size_t;

        static auto get_default() -> ptr_context;

    private:
        static std::weak_ptr<context> m_default;

        size_t m_frame_width    = 0;
        size_t m_frame_height   = 0;

        graphics::vec4      m_colors[8];
        text::ptr_renderer  m_font;

        graphics::flat::solid_buffer        m_solid_buffer;
        graphics::flat::sprite_buffer       m_sprite_buffer;
        graphics::flat::ptr_solid_shader    m_solid_shader;
        graphics::flat::ptr_sprite_shader   m_sprite_shader;
    };
}
