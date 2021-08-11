/*  laplace/format/f_image.cpp
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

#include "image.h"

#include "binary.h"
#include "buffer.h"
#include "text.h"

namespace laplace::format::image {
  using std::make_shared, core::family;

  auto extract(const_pack_type pack) -> image::data_type {
    auto result = make_shared<graphics::image>();

    sl::index width  = pack["width"].get_uint();
    sl::index height = pack["height"].get_uint();
    sl::index depth  = pack["depth"].get_uint();

    result->set_size(width, height, depth);

    auto pixels = pack["pixels"].get_bytes();

    if (pixels.size_bytes() == result->get_size_bytes()) {
      copy(pixels.begin(), pixels.end(), result->data());
    } else {
      result.reset();
    }

    return result;
  }

  auto package(const_data_type data) -> pack_type {
    auto result = make_shared<family>();

    (*result)["width"]  = data.get_width();
    (*result)["height"] = data.get_height();
    (*result)["depth"]  = data.get_depth();

    (*result)["pixels"] = span_cbyte(
        data.get_data(), data.get_data() + data.get_size_bytes());

    return result;
  }

  auto decode(fn_read read) -> image::data_type {
    data_type result;

    if (read) {
      buffer buf(read);

      auto buffering = [&buf](sl::index n) -> vbyte {
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
