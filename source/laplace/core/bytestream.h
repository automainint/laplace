/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_CORE_BYTESTREAM_H
#define LAPLACE_CORE_BYTESTREAM_H

#include "defs.h"
#include <sstream>

namespace laplace {
  class bytestreambuf : public std::streambuf {
  public:
    bytestreambuf(span_cbyte bytes) noexcept {
      auto p = const_cast<char *>(
          reinterpret_cast<char const *>(bytes.data()));
      this->setg(p, p, p + bytes.size());
    }

  protected:
    auto seekoff(off_type off, std::ios_base::seekdir dir,
                 std::ios_base::openmode which) -> pos_type override {
      if (dir == std::ios_base::cur)
        gbump(static_cast<int>(off));
      else if (dir == std::ios_base::end)
        setg(eback(), egptr() + off, egptr());
      else if (dir == std::ios_base::beg)
        setg(eback(), eback() + off, egptr());
      return gptr() - eback();
    }

    auto seekpos(pos_type sp, std::ios_base::openmode which)
        -> pos_type override {

      return seekoff(sp - pos_type(off_type(0)), std::ios_base::beg,
                     which);
    }
  };

  /*  span<uint8_t const> wrapper for std::istream.
   */
  class ibytestream final : private bytestreambuf,
                            public std::istream {
  public:
    ibytestream(span_cbyte bytes) noexcept :
        bytestreambuf(bytes), std::istream(
                                  static_cast<std::streambuf *>(
                                      this)) { }
  };
}

#endif
