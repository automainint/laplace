/*  laplace/format/f_utils.cpp
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

#include "utils.h"

namespace laplace::format {
  using std::istream, std::ostream;

  auto wrap(istream &in) noexcept -> fn_read {
    return [&in](sl::index n) -> vbyte {
      if (!in) {
        return {};
      }

      auto v = vbyte(n);
      in.read(reinterpret_cast<char *>(v.data()), n);
      v.resize(static_cast<sl::index>(in.gcount()));
      return v;
    };
  }

  auto wrap(ostream &out) noexcept -> fn_write {
    return [&out](span_cbyte data) -> sl::index {
      if (!out) {
        return 0;
      }

      out.write(
          reinterpret_cast<const char *>(data.data()), data.size());

      return data.size();
    };
  }
}
