/*  laplace/format/f_model.cpp
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

#include "binary.h"
#include "buffer.h"
#include "model.h"
#include "text.h"
#include <iostream>

namespace laplace::format::model {
  using std::make_shared, core::family;

  auto extract(const_pack_type pack) -> model::data_type {
    error(__FUNCTION__, "Not implemented.");
    return make_shared<render::model>();
  }

  auto package(const_data_type data) -> pack_type {
    error(__FUNCTION__, "Not implemented.");
    return make_shared<family>();
  }

  auto decode(fn_read read) -> model::data_type {
    data_type result;

    if (read) {
      buffer buf(read);

      auto buffering = [&buf](size_t n) -> vbyte {
        return buf.read(n);
      };

      auto pack = binary::decode(buffering);

      if (!pack) {
        buf.restore();
        pack = text::decode(buffering);
      }

      if (pack) {
        result = extract(*pack);
      }
    }

    return result;
  }

  auto encode(fn_write write, const_data_type data, mode encoding_mode)
      -> bool {
    bool result = false;

    if (write) {
      auto pack = package(data);

      if (pack) {
        if (encoding_mode == mode::text) {
          result = text::encode(write, *pack);
        } else /*if (encoding_mode == mode::binary)*/
        {
          result = binary::encode(write, *pack);
        }
      }
    }

    return result;
  }
}
