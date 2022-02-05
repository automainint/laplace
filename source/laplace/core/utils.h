/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_core_utils_h
#define laplace_core_utils_h

#include "bytestream.h"
#include "defs.h"
#include "string.h"
#include "utf8.h"

namespace laplace {
  /*  Convert unsigned integer value to an index.
   *  All indices are sl::index values.
   */
  [[nodiscard]] constexpr auto as_index(
      unsigned char const value, sl::index const invalid = -1,
      bool const silent = false) noexcept -> sl::index;

  /*  Convert unsigned integer value to an index.
   *  All indices are sl::index values.
   */
  [[nodiscard]] constexpr auto as_index(
      unsigned short const value, sl::index const invalid = -1,
      bool const silent = false) noexcept -> sl::index;

  /*  Convert unsigned integer value to an index.
   *  All indices are sl::index values.
   */
  [[nodiscard]] constexpr auto as_index(
      unsigned int const value, sl::index const invalid = -1,
      bool const silent = false) noexcept -> sl::index;

  /*  Convert unsigned integer value to an index.
   *  All indices are sl::index values.
   */
  [[nodiscard]] constexpr auto as_index(
      unsigned long const value, sl::index const invalid = -1,
      bool const silent = false) noexcept -> sl::index;

  /*  Convert 64-bit unsigned integer value to an index.
   *  All indices are sl::index values.
   */
  [[nodiscard]] constexpr auto as_index(
      unsigned long long const value, sl::index const invalid = -1,
      bool const silent = false) noexcept -> sl::index;

  /*  Convert integer value to an index.
   *  All indices are sl::index values.
   */
  [[nodiscard]] constexpr auto as_index(
      signed char const value, sl::index const invalid = -1,
      bool const silent = false) noexcept -> sl::index;

  /*  Convert integer value to an index.
   *  All indices are sl::index values.
   */
  [[nodiscard]] constexpr auto as_index(
      signed short const value, sl::index const invalid = -1,
      bool const silent = false) noexcept -> sl::index;

  /*  Convert integer value to an index.
   *  All indices are sl::index values.
   */
  [[nodiscard]] constexpr auto as_index(
      signed int const value, sl::index const invalid = -1,
      bool const silent = false) noexcept -> sl::index;

  /*  Convert integer value to an index.
   *  All indices are sl::index values.
   */
  [[nodiscard]] constexpr auto as_index(
      signed long const value, sl::index const invalid = -1,
      bool const silent = false) noexcept -> sl::index;

  /*  Convert 64-bit integer value to an index.
   *  All indices are sl::index values.
   */
  [[nodiscard]] constexpr auto as_index(
      signed long long const value, sl::index const invalid = -1,
      bool const silent = false) noexcept -> sl::index;
}

#include "utils.impl.h"

#endif
