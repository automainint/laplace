#pragma once

#include <functional>
#include <cstdint>

namespace laplace
{
    using event_init        = std::function<void ()>;
    using event_cleanup     = std::function<void ()>;
    using event_frame       = std::function<void (size_t delta_msec)>;
    using event_size        = std::function<void (size_t frame_width, size_t frame_height)>;
    using event_focus       = std::function<void (bool state)>;
    using event_drop_file   = std::function<void (std::wstring_view file_name)>;
    using event_key_down    = std::function<void (int code)>;
    using event_wheel       = std::function<void (int wheel_delta)>;
}
