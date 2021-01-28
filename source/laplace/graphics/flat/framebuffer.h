#pragma once

#include "../texture.h"
#include <functional>

namespace laplace::graphics::flat
{
    class framebuffer
    {
    public:
        texture color_texture;

        framebuffer();
        ~framebuffer();

        void set_size(size_t width, size_t height);

        void render(std::function<void()> op);

    private:
        uint32_t m_id;
        uint32_t m_width;
        uint32_t m_height;
    };

    using ptr_framebuffer = std::shared_ptr<framebuffer>;
}
