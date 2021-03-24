#pragma once

#include "renderer.h"
#include "painter.h"
#include "../../graphics/texture.h"
#include <array>

namespace laplace::ui::text
{
    /*  Buffered 16x16 font.
     */
    class buffer final : public renderer
    {
    public:
        static constexpr size_t char_count = 256;

        ~buffer() final = default;

        void build(ref_painter paint);
        void build_monospace(ref_painter paint, int top, size_t char_width, size_t char_height);
        void build_monospace(graphics::cref_image img, int top, size_t char_width, size_t char_height);

        auto adjust(std::u8string_view text) -> area final;
        void render(int x, int y, std::u8string_view text) final;

    private:
        int     m_cell_top      = 0;
        size_t  m_cell_width    = 0;
        size_t  m_cell_height   = 0;

        std::array<area, char_count> m_chars;
        graphics::ptr_texture m_texture = std::make_shared<graphics::texture>();
    };
}
