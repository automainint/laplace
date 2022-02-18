/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_CORE_UTILS_H
#define LAPLACE_CORE_UTILS_H

#include "defs.h"

namespace laplace {
  /*  Convert unsigned integer value to an index.
   *  All indices are sl::index values.
   */
  [[nodiscard]] constexpr auto as_index(
      unsigned char value, sl::index invalid = -1) noexcept
      -> sl::index;

  /*  Convert unsigned integer value to an index.
   *  All indices are sl::index values.
   */
  [[nodiscard]] constexpr auto as_index(
      unsigned short value, sl::index invalid = -1) noexcept
      -> sl::index;

  /*  Convert unsigned integer value to an index.
   *  All indices are sl::index values.
   */
  [[nodiscard]] constexpr auto as_index(
      unsigned int value, sl::index invalid = -1) noexcept
      -> sl::index;

  /*  Convert unsigned integer value to an index.
   *  All indices are sl::index values.
   */
  [[nodiscard]] constexpr auto as_index(
      unsigned long value, sl::index invalid = -1) noexcept
      -> sl::index;

  /*  Convert 64-bit unsigned integer value to an index.
   *  All indices are sl::index values.
   */
  [[nodiscard]] constexpr auto as_index(
      unsigned long long value, sl::index invalid = -1) noexcept
      -> sl::index;

  /*  Convert integer value to an index.
   *  All indices are sl::index values.
   */
  [[nodiscard]] constexpr auto as_index(
      signed char value, sl::index invalid = -1) noexcept
      -> sl::index;

  /*  Convert integer value to an index.
   *  All indices are sl::index values.
   */
  [[nodiscard]] constexpr auto as_index(
      signed short value, sl::index invalid = -1) noexcept
      -> sl::index;

  /*  Convert integer value to an index.
   *  All indices are sl::index values.
   */
  [[nodiscard]] constexpr auto as_index(
      signed int value, sl::index invalid = -1) noexcept -> sl::index;

  /*  Convert integer value to an index.
   *  All indices are sl::index values.
   */
  [[nodiscard]] constexpr auto as_index(
      signed long value, sl::index invalid = -1) noexcept
      -> sl::index;

  /*  Convert 64-bit integer value to an index.
   *  All indices are sl::index values.
   */
  [[nodiscard]] constexpr auto as_index(
      signed long long value, sl::index invalid = -1) noexcept
      -> sl::index;
}

#include "utils.impl.h"

#endif
