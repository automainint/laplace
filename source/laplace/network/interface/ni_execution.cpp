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
    return in.is_desync && in.get_current_time && in.do_server_seed &&
           in.do_server_reserve && in.do_reserve &&
           in.do_wait_loading && in.do_loading && in.do_apply &&
           in.do_perform && in.do_schedule && in.do_cleanup;
  }

  auto blank_execution_interface() noexcept -> execution_interface {
    auto const blank_action = [](span_cbyte) {};

    return { .is_desync = []() -> bool { return false; },
             .get_current_time = []() -> sl::time { return 0; },
             .do_server_seed    = blank_action,
             .do_server_reserve = blank_action,
             .do_reserve        = []() -> sl::index { return -1; },
             .do_wait_loading = []() -> bool { return false; },
             .do_loading = [](span_cbyte) {},
             .do_apply   = [](span_cbyte) -> sl::time { return 0; },
             .do_perform  = [](span_cbyte) {},
             .do_schedule = [](sl::time) {},
             .do_cleanup  = []() {} };
  }
}
