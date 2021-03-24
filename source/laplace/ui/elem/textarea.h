#pragma once

#include "../widget.h"
#include "../text/renderer.h"

namespace laplace::ui::elem
{
    class textarea final : public widget
    {
    public:
        static constexpr int default_line_height = 30;

        ~textarea() final = default;

        void render() final;

        void set_renderer(text::ptr_renderer renderer);
        void set_text(std::u8string_view text);
        void set_line_height(int line_height);

    private:
        void textarea_render();

        text::ptr_renderer  m_renderer      = text::renderer::get_default();
        std::u8string       m_text;
        int                 m_line_height   = default_line_height;
    };

    using ptr_textarea = std::shared_ptr<textarea>;
    using vptr_textarea = std::vector<ptr_textarea>;
}
