/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "execution.h"

namespace laplace::network {
  auto check_execution_interface(
      execution_interface const &in) noexcept -> bool {
    return in.actor_reserve && in.actor_create && in.actor_remove &&
           in.do_perform && in.do_seed && in.do_loading &&
           in.do_apply && in.do_schedule && in.is_desync &&
           in.is_ready;
  }

  auto blank_execution_interface() noexcept -> execution_interface {
    return { .actor_reserve = [](span_cbyte) {},
             .actor_create  = []() -> sl::index { return -1; },
             .actor_remove = [](sl::index) {},
             .do_perform   = [](span_cbyte) {},
             .do_seed      = [](span_cbyte) {},
             .do_loading   = [](span_cbyte) {},
             .do_apply     = [](span_cbyte) -> sl::time { return 0; },
             .do_schedule = [](sl::time) {},
             .is_desync   = []() -> bool { return false; },
             .is_ready = []() -> bool { return true; } };
  }
}
