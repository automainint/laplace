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

#include <string>

namespace laplace {
  void set_verbose(bool is_verbose);
  auto is_verbose() -> bool;

  void log(std::string_view s);
  void log(std::u8string_view s);

  /*  TODO
   *  Use source_location.
   */
  void error_(std::string_view message, std::string_view loc = {});
  void error_(std::u8string_view message, std::string_view loc = {});

  void verb(std::string_view s);
  void verb(std::u8string_view s);
}

#endif
