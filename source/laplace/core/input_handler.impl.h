/*  laplace/core/input_handler.impl.h
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

#ifndef laplace_core_input_handler_impl_h
#define laplace_core_input_handler_impl_h

namespace laplace::core {
  constexpr auto is_key_down(input_event const &ev) noexcept -> bool {
    if constexpr (!_unsafe) {
      if (ev.key == keys::key_wheel) {
        error_("Not key event.", __FUNCTION__);
        return false;
      }
    }

    return ev.delta > 0;
  }

  constexpr auto is_key_up(input_event const &ev) noexcept -> bool {
    if constexpr (!_unsafe) {
      if (ev.key == keys::key_wheel) {
        error_("Not key event.", __FUNCTION__);
        return false;
      }
    }

    return ev.delta < 0;
  }

  constexpr auto get_wheel_delta(input_event const &ev) noexcept
      -> sl::index {
    if constexpr (!_unsafe) {
      if (ev.key != keys::key_wheel) {
        error_("Not wheel event.", __FUNCTION__);
        return 0;
      }
    }

    return ev.delta;
  }
}

#endif
