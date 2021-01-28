#pragma once

#include "../../graphics/image.h"
#include <string>

namespace laplace::ui::text
{
     /*  Rendering text into image.
     */
    class painter
    {
    public:
        struct area
        {
            int top         = 0;
            size_t width    = 0;
            size_t height   = 0;
        };

        using ptr_painter = std::shared_ptr<painter>;

        static constexpr int    default_lcd_char_top    = 6;
        static constexpr size_t default_lcd_char_width  = 5;
        static constexpr size_t default_lcd_char_height = 7;
        static const uint64_t   default_lcd_bits[ ];

        static constexpr auto default_color = graphics::rgba(250, 240, 255, 255);

        virtual ~painter() = default;

        virtual auto adjust(std::u8string_view text) -> area;
        virtual void print(graphics::ref_image img, int x, int y, std::u8string_view text);

        static auto get_default() -> ptr_painter;

    private:
        static std::weak_ptr<painter> m_default;
    };

    using ref_painter = painter &;
    using cref_painter = const painter &;

    using ptr_painter = painter::ptr_painter;
}
