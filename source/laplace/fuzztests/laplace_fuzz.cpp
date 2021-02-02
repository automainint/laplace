/*  laplace/fuzztests/laplace_fuzz.cpp
 *
 *      LLVM libFuzzer entry.
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

#include <cstdint>

extern "C" int LLVMFuzzerTestOneInput(
    const uint8_t *data, size_t size) {
  /*  TODO
   *  data -> factory => impacts -> solver -> world
   */
  return 0;
}
