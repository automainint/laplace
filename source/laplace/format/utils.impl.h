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
  inline auto wrap(std::istream &in) -> fn_read {
    return [&in](sl::index n) -> vbyte {
      if (in) {
        auto v = vbyte(n);
        auto p = in.tellg();
        in.read(reinterpret_cast<char *>(v.data()),
                static_cast<ptrdiff_t>(n));
        v.resize(static_cast<sl::index>(in.tellg() - p));
        return v;
      }

      return {};
    };
  }

  inline auto wrap(std::ostream &out) -> fn_write {
    return [&out](span_cbyte data) -> sl::index {
      if (out) {
        auto p = out.tellp();
        out.write(reinterpret_cast<const char *>(data.data()),
                  static_cast<ptrdiff_t>(data.size()));
        return static_cast<sl::index>(out.tellp() - p);
      }

      return 0;
    };
  }
}

#endif
