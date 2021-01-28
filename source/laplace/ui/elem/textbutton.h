#pragma once

#include "button.h"
#include "../text/renderer.h"

namespace laplace::ui::elem
{
    class textbutton final : public button
    {
    public:
        ~textbutton() final = default;

        void render() final;

        void set_text(std::u8string_view text);

    private:
        std::u8string m_text;
    };

    using ptr_textbutton = std::shared_ptr<textbutton>;
}
