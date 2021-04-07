/*  laplace/core/slib.h
 *
 *      The Laplace support library.
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

#ifndef laplace_core_slib_h
#define laplace_core_slib_h

#include "log.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#ifdef __cpp_exceptions
#  define laplace_try() try {
#  define laplace_catch()                                  \
    }                                                      \
    catch (std::exception & e) {                           \
      laplace::error_(e.what(), __FUNCTION__);             \
    }                                                      \
    catch (...) {                                          \
      laplace::error_("Unknown exception.", __FUNCTION__); \
    }
#  define laplace_catch_unreachable() \
    }                                 \
    catch (...) {                     \
      laplace::sl::_unreachable();    \
    }
#else
#  define laplace_try() {
#  define laplace_catch() }
#  define laplace_catch_unreachable() }
#endif

namespace laplace::sl {
  /*  TODO
   *  Small vector optimization.
   */
  template <typename type_, typename alloc_ = std::allocator<type_>>
  using vector = std::vector<type_, alloc_>;

  using index = ptrdiff_t;

  [[noreturn]] inline void _unreachable() noexcept { }
}

#endif
