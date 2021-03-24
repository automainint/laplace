#pragma once

#include "../widget.h"
#include "../../graphics/texture.h"

namespace laplace::ui::elem
{
    class canvas final : public widget
    {
    public:
        canvas();
        ~canvas() final = default;

        void set_context(ptr_context cont);

        void render() final;

        auto image() -> graphics::ref_image;
        auto get_image() const -> graphics::cref_image;

    private:
        graphics::image     m_image;
        graphics::texture   m_texture;

        ptr_context m_context;
    };

    using ptr_canvas = std::shared_ptr<canvas>;
}
