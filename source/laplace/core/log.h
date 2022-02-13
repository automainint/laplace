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
//#include <source_location>
#include <functional>
#include <string>

namespace laplace {
  enum class log_event { message, warning, error, fatal };

  using fn_log = std::function<void(log_event, std::string_view,
                                    std::string_view)>;

  void disable_log() noexcept;
  void setup_log(fn_log const &write) noexcept;
  void set_verbose(bool is_verbose) noexcept;
  auto is_verbose() noexcept -> bool;

  void log(log_event event, std::string_view message,
           std::string_view origin) noexcept;
  void log(log_event event, std::u8string_view message,
           std::u8string_view origin) noexcept;

  void verb(std::string_view message,
            std::string_view origin) noexcept;
  void verb(std::u8string_view message,
            std::u8string_view origin) noexcept;

  /*  TODO
   *  Use source_location.
   */
  void error_(std::string_view message,
              std::string_view origin) noexcept;
  void error_(std::u8string_view message,
              std::u8string_view origin) noexcept;
}

#endif
