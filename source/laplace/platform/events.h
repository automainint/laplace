/*  laplace/platform/events.h
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_platform_events_h
#define laplace_platform_events_h

#include <cstdint>
#include <functional>

namespace laplace {
  using event_init      = std::function<void()>;
  using event_cleanup   = std::function<void()>;
  using event_frame     = std::function<void(uint64_t delta_msec)>;
  using event_size      = std::function<void( //
      size_t frame_width,                //
      size_t frame_height)>;
  using event_focus     = std::function<void(bool state)>;
  using event_drop_file = std::function<void( //
      std::wstring_view file_name)>;
  using event_key_down  = std::function<void(int code)>;
  using event_wheel     = std::function<void(int wheel_delta)>;
}

#endif
