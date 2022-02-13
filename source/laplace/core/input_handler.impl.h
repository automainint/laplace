/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_CORE_INPUT_HANDLER_IMPL_H
#define LAPLACE_CORE_INPUT_HANDLER_IMPL_H

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
