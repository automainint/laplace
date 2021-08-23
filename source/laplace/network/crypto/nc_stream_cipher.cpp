/*  laplace/network/crypto/nc_stream_cipher.cpp
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

#include "stream_cipher.h"

#include "../../core/serial.h"
#include "../../core/utils.h"
#include "../transfer.h"

namespace laplace::network::crypto {
  using std::min, std::tuple, std::span, serial::rd, serial::wr;

  const sl::whole stream_cipher::block_size        = 64;
  const sl::whole stream_cipher::max_offset_change = 10000;

  auto stream_cipher::encrypt(span_cbyte bytes) -> vbyte {

    uint8_t block[block_size] = {};

    auto buf = vbyte {};
    auto n   = sl::index {};

    while (n < bytes.size()) {
      const auto size = min<sl::index>(block_size, bytes.size() - n);

      const auto n_buf = buf.size();
      buf.resize(n_buf + n_data + block_size);

      wr<uint64_t>(buf, n_buf + n_offset, m_enc_offset);
      wr<uint64_t>(buf, n_buf + n_size, size);

      memcpy(block, bytes.data() + n, size);
      memset(block + size, 0, block_size - size);

      if (!do_encrypt(block,
                      { buf.data() + n_buf + n_data, block_size }))
        break;

      const auto sum = transfer::check_sum(
          { buf.data() + n_buf + n_data,
            static_cast<span_cbyte::size_type>(size) });

      wr<uint64_t>(buf, n_buf + n_sum, sum);

      m_enc_offset += block_size;
      n += size;
    }

    return buf;
  }

  auto stream_cipher::decrypt(span_cbyte bytes) -> vbyte {

    reset_loss_count();

    uint8_t block[block_size] = {};

    auto buf = vbyte {};
    auto n   = sl::index {};

    while (n < bytes.size()) {
      const auto offset = as_index(rd<uint64_t>(bytes, n + n_offset));
      const auto size   = as_index(rd<uint64_t>(bytes, n + n_size));

      if (scan({ bytes.begin() + n, bytes.end() })) {

        if (offset != m_dec_offset) {
          if (!rewind_decryption()) {
            add_bytes_lost(bytes.size() - n);
            break;
          }

          m_dec_offset = 0;

          if (!pass_decryption(offset)) {
            add_bytes_lost(bytes.size() - n);
            break;
          }

          m_dec_offset = offset;
        }

        n += n_data;

        if (!do_decrypt({ bytes.data() + n, block_size }, block)) {
          add_bytes_lost(bytes.size() - n);
          break;
        }

        buf.insert(buf.end(), block, block + size);

        m_dec_offset += block_size;
        n += block_size;

      } else {
        add_bytes_lost(1);
        n++;
      }
    }

    return buf;
  }

  auto stream_cipher::do_encrypt(span_cbyte src, span<uint8_t> dst)
      -> bool {
    error_("Not implemented.", __FUNCTION__);
    return false;
  }

  auto stream_cipher::do_decrypt(span_cbyte src, span<uint8_t> dst)
      -> bool {
    error_("Not implemented.", __FUNCTION__);
    return false;
  }

  auto stream_cipher::rewind_decryption() -> bool {
    error_("Not implemented.", __FUNCTION__);
    return false;
  }

  auto stream_cipher::pass_decryption(sl::index offset) -> bool {
    const uint8_t src[block_size] = { 0 };
    uint8_t       dst[block_size] = { 0 };

    for (sl::index i = 0; i < offset; i += block_size) {
      const auto delta = min<sl::index>(block_size, offset - i);

      if (!do_decrypt(
              { src, static_cast<span_cbyte::size_type>(delta) },
              { dst, static_cast<span_byte::size_type>(delta) })) {

        return false;
      }
    }

    return true;
  }

  auto stream_cipher::scan(span_cbyte data) const -> bool {
    const auto offset = rd<uint64_t>(data, n_offset);
    const auto sum    = rd<uint64_t>(data, n_sum);
    const auto size   = rd<uint64_t>(data, n_size);

    if (offset > m_dec_offset + max_offset_change) {
      return false;
    }

    if (m_dec_offset > offset + max_offset_change) {
      return false;
    }

    if (block_size + n_data > data.size()) {
      return false;
    }

    if (size > block_size) {
      return false;
    }

    if (sum != transfer::check_sum({ data.data() + n_data, size })) {
      verb_error("Stream cipher", "Wrong check sum.");
      return false;
    }

    return true;
  }
}
