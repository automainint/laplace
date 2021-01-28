#pragma once

#include "widget.h"
#include "context.h"
#include "../graphics/flat/framebuffer.h"

namespace laplace::ui
{
    class frame final : public widget
    {
    public:
        frame();
        ~frame() final = default;

        void set_context(ptr_context cont);

        void render() final;

    private:
        ptr_context m_context;
        graphics::flat::framebuffer m_buffer;
    };

    using ptr_frame = std::shared_ptr<frame>;
}
