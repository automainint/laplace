/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_core_log_h
#define laplace_core_log_h

#include "options.h"
#include <functional>
#include <string>

namespace laplace {
  enum class log_event { verbose, message, warning, error, fatal };

  using log_handler =
      std::function<void(log_event event, std::string_view message,
                         std::string_view origin)>;

  void setup_global_log(log_handler const &handler) noexcept;
  auto get_global_log() noexcept -> log_handler;
  void set_verbose(bool is_verbose) noexcept;
  auto is_verbose() noexcept -> bool;
}

#endif
