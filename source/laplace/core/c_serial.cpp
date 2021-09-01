/*  laplace/core/c_serial.cpp
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

#include "serial.h"

namespace laplace::serial {
  static_assert(_swap_bytes(uint8_t { 0x01 }) == uint8_t { 0x01 });

  static_assert(_swap_bytes(uint16_t { 0x0102 }) ==
                uint16_t { 0x0201 });

  static_assert(_swap_bytes(uint32_t { 0x01020304 }) ==
                uint32_t { 0x04030201 });

  static_assert(_swap_bytes(0x0102030405060708) ==
                0x0807060504030201);

  static_assert(_swap_bytes(int8_t { 0x01 }) == int8_t { 0x01 });

  static_assert(_swap_bytes(int16_t { 0x0102 }) ==
                int16_t { 0x0201 });

  static_assert(_swap_bytes(int32_t { 0x01020304 }) ==
                int32_t { 0x04030201 });

  static_assert(_swap_bytes(int64_t { 0x0102030405060708 }) ==
                int64_t { 0x0807060504030201 });
}
