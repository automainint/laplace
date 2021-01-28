#pragma once

#include "../widget.h"

namespace laplace::ui::elem
{
    class panel : public widget
    {
    public:
        struct state
        {
            size_t      level = 0;
            ui::rect    rect;
            bool        is_enabled = true;
        };

        ~panel() override = default;

        void set_context(ptr_context cont);

        void render() override;

        auto get_state() const -> state;

    protected:
        ptr_context m_context = widget::get_default_context();
    };

    using ptr_panel = std::shared_ptr<panel>;
}
