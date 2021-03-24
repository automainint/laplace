/*  laplace/format/utils.impl.h
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

#ifndef laplace_format_utils_impl_h
#define laplace_format_utils_impl_h

namespace laplace::format {
  inline auto wrap(ref_istream in) -> fn_read {
    return [&in](size_t n) -> vbyte {
      if (in) {
        vbyte v(n);
        auto  p = in.tellg();
        in.read(reinterpret_cast<char *>(v.data()),
                static_cast<ptrdiff_t>(n));
        v.resize(static_cast<size_t>(in.tellg() - p));
        return v;
      }

      return {};
    };
  }

  inline auto wrap(ref_ostream out) -> fn_write {
    return [&out](cref_vbyte data) -> size_t {
      if (out) {
        auto p = out.tellp();
        out.write(reinterpret_cast<const char *>(data.data()),
                  static_cast<ptrdiff_t>(data.size()));
        return static_cast<size_t>(out.tellp() - p);
      }

      return 0;
    };
  }
}

#endif
