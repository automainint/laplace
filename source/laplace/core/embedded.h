/*  laplace/core/embedded.h
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

#ifndef laplace_core_embedded_
#define laplace_core_embedded_

#include "defs.h"

namespace laplace::embedded {
  [[nodiscard]] auto scan(std::wstring_view file_name) noexcept
      -> bool;
  [[nodiscard]] auto exists(
      std::wstring_view file_name,
      log_handler       log = get_global_log()) noexcept -> bool;
  [[nodiscard]] auto open(std::wstring_view file_name,
                          log_handler log = get_global_log()) noexcept
      -> span_cbyte;
}

#endif
