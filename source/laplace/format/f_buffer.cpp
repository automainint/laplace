/*  laplace/format/f_buffer.cpp
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

#include "buffer.h"

#include <algorithm>

namespace laplace::format {
  buffer::buffer(fn_read read) {
    m_read   = read;
    m_offset = 0;
  }

  auto buffer::read(sl::whole count) -> vbyte {
    if (count < 0) {
      error_("Invalid count.", __FUNCTION__);
      return {};
    }

    auto piece = m_data.size() - m_offset;

    if (count > piece) {
      auto v = m_read(count - piece);
      m_data.insert(m_data.end(), v.begin(), v.end());
    }

    auto p = m_data.begin() + m_offset;
    m_offset += m_data.size();

    return vbyte(p, p + count);
  }

  void buffer::keep() {
    m_offset = 0;
    m_data.clear();
  }

  void buffer::restore() {
    m_offset = 0;
  }
}
