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
    buffer(fn_read read);
    ~buffer();

    auto read(size_t count) -> vbyte;

    void keep();
    void restore();

  private:
    fn_read m_read;
    size_t  m_offset;
    vbyte   m_data;
  };
}

#endif
