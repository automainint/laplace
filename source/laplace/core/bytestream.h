/*  laplace/core/bytestream.h
 *
 *      Byte stream wrapper for std::istream.
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

#ifndef laplace_core_bytestream_h
#define laplace_core_bytestream_h

#include "defs.h"
#include <sstream>

namespace laplace {
  class bytestreambuf final : public std::stringbuf {
  public:
    bytestreambuf(const bytestreambuf &) = delete;
    auto operator=(const bytestreambuf &) -> bytestreambuf & = delete;

    bytestreambuf(cref_vbyte bytes) {
      _Init(reinterpret_cast<const char *>(bytes.data()),
            bytes.size(), 2 /* _Constant from basic_stringbuf. */
      );
    }

    ~bytestreambuf() final = default;
  };

  /*  span<const uint8_t> wrapper for std::istream.
   */
  class ibytestream final : public std::istream {
  public:
    ibytestream(const ibytestream &) = delete;
    auto operator=(const ibytestream &) -> ibytestream & = delete;

    ibytestream(cref_vbyte bytes) :
        std::istream(new bytestreambuf(bytes)) { }

    ~ibytestream() final {
      delete rdbuf();
    }
  };
}

#endif
