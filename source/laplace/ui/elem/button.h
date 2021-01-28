#pragma once

#include "panel.h"

namespace laplace::ui::elem
{
    using event_button_click = std::function<void (ptr_widget)>;

    class button : public panel
    {
    public:
        struct state : panel::state
        {
            bool is_pressed;
            bool has_cursor;
        };

        struct update_result
        {
            bool event_status;
            bool is_pressed;
            bool has_cursor;
        };

        ~button() override;

        void on_click(event_button_click ev);

        auto tick(size_t delta_msec, platform::ref_input in) -> bool final;
        void render() override;

        void set_pressed(bool is_pressed);
        void set_cursor(bool has_cursor);

        auto is_pressed() const -> bool;
        auto has_cursor() const -> bool;

        auto get_state() const -> state;

        static auto update(
            ptr_widget object,
            state button_state,
            event_button_click on_button_click,
            platform::ref_input in
        ) -> update_result;

    private:
        auto button_tick(platform::ref_input in) -> bool;

        event_button_click m_on_click   = [](ptr_widget) { };
        bool m_is_pressed               = false;
        bool m_has_cursor               = false;
    };

    using ptr_button = std::shared_ptr<button>;
}
