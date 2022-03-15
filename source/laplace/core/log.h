/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_CORE_LOG_H
#define LAPLACE_CORE_LOG_H

#include "options.h"
#include <functional>
#include <string>

namespace laplace {
  enum class log_event { verbose, message, warning, error, fatal };

  using log_handler =
      std::function<void(log_event event, std::string_view message,
                         std::string_view origin)>;

  void setup_global_log(log_handler const &handler) noexcept;
  [[nodiscard]] auto get_global_log() noexcept -> log_handler;
  [[nodiscard]] auto get_blank_log() noexcept -> log_handler;
  void               set_verbose(bool is_verbose) noexcept;
  [[nodiscard]] auto is_verbose() noexcept -> bool;
}

#endif
