/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_network_interface_log_h
#define laplace_network_interface_log_h

#include "../defs.h"

namespace laplace::network {
  using fn_log_print = std::function<void(std::string_view)>;
  using fn_log_queue = std::function<void(sl::index, span_cbyte)>;
  using fn_log_slot =
      std::function<void(sl::index, sl::index, span_cbyte)>;
  using fn_log_not_allowed =
      std::function<void(sl::index, span_cbyte)>;
  using fn_log_dump = std::function<void(span_cbyte)>;

  struct log_interface {
    fn_log_print       print;
    fn_log_queue       queue;
    fn_log_slot        slot;
    fn_log_not_allowed not_allowed;
    fn_log_dump        dump;
  };

  [[nodiscard]] auto check_log_interface(
      log_interface const &in) noexcept -> bool;

  [[nodiscard]] auto blank_log_interface() noexcept -> log_interface;
}

#endif
