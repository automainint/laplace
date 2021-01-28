#pragma once

#include "painter.h"
#include "ttf.h"
#include <memory>

namespace laplace::ui::text
{
    /*  TTF font renderer.
     */
    class font final : public painter
    {
    public:
        using cref_bitmap = const FT_Bitmap &;

        ~font() final = default;

        void load(std::u8string_view file_name);
        void set_size(size_t width, size_t height);

        auto adjust(std::u8string_view text) -> area final;
        void print(graphics::ref_image img, int x, int y, std::u8string_view text) final;

        auto get_color() const -> graphics::cref_pixel;

    private:
        static void draw(graphics::ref_image img, int x0, int y0, cref_bitmap bitmap, graphics::cref_pixel color);
        static auto blend(graphics::cref_pixel s, graphics::cref_pixel d, uint8_t factor) -> graphics::pixel;

        graphics::pixel m_color = default_color;
        ttf             m_face;
    };

    using ref_font = font &;
    using cref_font = const font &;
    using ptr_font = std::shared_ptr<font>;
}
