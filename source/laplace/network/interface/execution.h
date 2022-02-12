/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_NETWORK_INTERFACE_EXECUTION_H
#define LAPLACE_NETWORK_INTERFACE_EXECUTION_H

#include "../defs.h"

namespace laplace::network {
  using fn_is_desync     = std::function<bool()>;
  using fn_is_ready      = std::function<bool()>;
  using fn_actor_reserve = std::function<void(span_cbyte)>;
  using fn_actor_create  = std::function<sl::index()>;
  using fn_actor_remove  = std::function<void(sl::index)>;
  using fn_do_seed       = std::function<void(span_cbyte)>;
  using fn_do_loading    = std::function<void(span_cbyte)>;
  using fn_do_apply      = std::function<sl::time(span_cbyte)>;
  using fn_do_perform    = std::function<void(span_cbyte)>;
  using fn_do_schedule   = std::function<void(sl::time)>;
  using fn_do_setup      = std::function<void()>;
  using fn_do_cleanup    = std::function<void()>;

  struct execution_interface {
    fn_is_desync     is_desync;
    fn_is_ready      is_ready;
    fn_actor_reserve actor_reserve;
    fn_actor_create  actor_create;
    fn_actor_remove  actor_remove;
    fn_do_seed       do_seed;
    fn_do_loading    do_loading;
    fn_do_apply      do_apply;
    fn_do_perform    do_perform;
    fn_do_schedule   do_schedule;
    fn_do_setup      do_setup;
    fn_do_cleanup    do_cleanup;
  };

  [[nodiscard]] auto check_execution_interface(
      execution_interface const &in) noexcept -> bool;

  [[nodiscard]] auto blank_execution_interface() noexcept
      -> execution_interface;
}

#endif
