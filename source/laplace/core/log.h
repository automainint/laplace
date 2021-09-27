/*  laplace/core/log.h
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

#ifndef laplace_core_log_h
#define laplace_core_log_h

//#include <source_location>
#include <functional>
#include <string>

namespace laplace {
  using fn_log = std::function<void(std::string_view)>;

  void setup_log(fn_log fn) noexcept;
  void set_verbose(bool is_verbose) noexcept;
  auto is_verbose() noexcept -> bool;

  void log(std::string_view s) noexcept;
  void log(std::u8string_view s) noexcept;

  /*  TODO
   *  Use source_location.
   */
  void error_(std::string_view message,
              std::string_view loc) noexcept;
  void error_(std::u8string_view message,
              std::string_view   loc) noexcept;

  void verb(std::string_view s) noexcept;
  void verb(std::u8string_view s) noexcept;

#ifndef LAPLACE_VERBOSE
  constexpr void set_verbose(bool is_verbose) noexcept { }

  constexpr auto is_verbose() noexcept -> bool {
    return false;
  }

  constexpr void verb(std::string_view s) noexcept { }
  constexpr void verb(std::u8string_view s) noexcept { }
#endif
}

#endif
