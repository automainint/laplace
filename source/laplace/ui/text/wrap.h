#pragma once

#include "renderer.h"
#include "painter.h"
#include "../context.h"
#include "../../graphics/texture.h"

namespace laplace::ui::text
{
    /*  Painter to Renderer wrapper.
     */
    class wrap final : public renderer
    {
    public:
        wrap(ptr_painter paint, ptr_context cont = context::get_default());
        ~wrap() override = default;

        void set_context(ptr_context cont);
        
        auto adjust(std::u8string_view text) -> area final;
        void render(int x, int y, std::u8string_view text) final;

    private:
        ptr_context m_context;
        ptr_painter m_paint;

        graphics::ptr_texture m_tex = std::make_shared<graphics::texture>();
    };
}
