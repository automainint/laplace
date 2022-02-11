/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "log_interface.h"

namespace laplace::network {
  auto check_log_interface(log_interface const &in) noexcept -> bool {
    return in.print && in.queue && in.slot && in.not_allowed && in.dump;
  }

  auto blank_log_interface() noexcept -> log_interface {
    return { .print       = [](std::string_view) {},
             .queue       = [](sl::index, span_cbyte) {},
             .slot        = [](sl::index, sl::index, span_cbyte) {},
             .not_allowed = [](sl::index, span_cbyte) {},
             .dump        = [](span_cbyte) {} };
  }
}
