#pragma once

#include "painter.h"

namespace laplace::ui::text
{
    /*  Rendering text as vertex array.
     */
    class renderer
    {
    public:
        using area = painter::area;

        using ptr_renderer = std::shared_ptr<renderer>;

        virtual ~renderer() = default;

        virtual auto adjust(std::u8string_view text) -> area;
        virtual void render(int x, int y, std::u8string_view text);

        static auto get_default() -> ptr_renderer;

    private:
        static std::weak_ptr<renderer> m_default;
    };

    using ref_renderer = renderer &;
    using cref_renderer = const renderer &;

    using ptr_renderer = renderer::ptr_renderer;
}
