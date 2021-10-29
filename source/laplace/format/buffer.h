/*  laplace/format/buffer.h
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

#ifndef laplace_format_buffer_h
#define laplace_format_buffer_h

#include "defs.h"

namespace laplace::format {
  class buffer {
  public:
    buffer(fn_read read) noexcept;
    ~buffer() noexcept = default;

    [[nodiscard]] auto read(sl::whole count) noexcept -> vbyte;

    void keep() noexcept;
    void restore() noexcept;

  private:
    fn_read   m_read;
    sl::index m_offset = 0;
    vbyte     m_data;
  };
}

#endif
