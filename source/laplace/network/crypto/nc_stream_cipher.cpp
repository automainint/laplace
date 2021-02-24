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

namespace laplace::network::crypto {
  using std::min, std::tuple, std::span;

  auto stream_cipher::encrypt(cref_vbyte bytes) -> vbyte {
    const auto offset = m_enc_offset;
    return chunk_pack(offset, chunk_encrypt(data_pack(bytes)));
  }

  auto stream_cipher::decrypt(cref_vbyte bytes) -> vbyte {
    vbyte result;

    for (size_t n = 0;           //
         chunk_scan(             //
             { bytes.data() + n, //
               bytes.size() - n });) {

      const auto [chunk, offset] = chunk_unpack({ bytes.data() + n, //
                                                  bytes.size() - n });

      const auto data = data_unpack(chunk_decrypt(offset, chunk));

      result.insert(result.end(), data.begin(), data.end());
      n += n_chunk_data + chunk.size();
    }

    return result;
  }

  auto stream_cipher::setup() -> bool {
    return false;
  }

  auto stream_cipher::do_encrypt(cref_vbyte src, span<uint8_t> dst)
      -> bool {
    return false;
  }

  auto stream_cipher::do_decrypt(cref_vbyte src, span<uint8_t> dst)
      -> bool {
    return false;
  }

  auto stream_cipher::rewind_decryption() -> bool {
    return false;
  }

  auto stream_cipher::pass_decryption(size_t offset) -> bool {
    const uint8_t src[block_size] = { 0 };
    uint8_t       dst[block_size] = { 0 };

    for (size_t i = 0; i < offset; i += block_size) {
      const size_t delta = min(block_size, offset - i);

      if (!do_decrypt({ src, delta }, { dst, delta }))
        return false;
    }

    return true;
  }

  auto stream_cipher::check_sum(cref_vbyte bytes) -> sum_type {
    sum_type sum = 0;

    for (size_t i = 0; i < bytes.size(); i += sizeof sum) {
      sum_type z = 0;

      memcpy(               //
          &z,               //
          bytes.data() + i, //
          min(sizeof z, bytes.size() - i));

      sum ^= z;
    }

    return sum;
  }

  auto stream_cipher::data_pack(cref_vbyte bytes) -> vbyte {
    const uint64_t size = bytes.size();
    const auto     sum  = check_sum(bytes);

    vbyte result(n_pack_data + bytes.size());
    memcpy(result.data() + n_pack_size, &size, sizeof size);
    memcpy(result.data() + n_pack_check_sum, &sum, sizeof sum);
    memcpy(result.data() + n_pack_data, bytes.data(), bytes.size());

    return result;
  }

  auto stream_cipher::data_unpack(cref_vbyte bytes) -> vbyte {
    if (bytes.size() < n_pack_data) {
      verb_error(__FUNCTION__, "Invalid data.");
      return {};
    }

    uint64_t size = 0;
    sum_type sum  = 0;

    memcpy(&size, bytes.data() + n_pack_size, sizeof size);
    memcpy(&sum, bytes.data() + n_pack_check_sum, sizeof sum);

    if (bytes.size() < n_pack_data + size) {
      verb_error(__FUNCTION__, "Invalid size.");
      return {};
    }

    if (check_sum({ bytes.data() + n_pack_data, size }) != sum) {
      verb("Stream cipher: WRONG CHECK SUM");
      return {};
    }

    vbyte result(size);
    memcpy(result.data(), bytes.data() + n_pack_data, size);
    return result;
  }

  auto stream_cipher::chunk_scan(cref_vbyte bytes) -> bool {
    if (bytes.empty())
      return false;

    if (bytes.size() < n_chunk_data) {
      verb_error(__FUNCTION__, "Invalid size.");
      return false;
    }

    sum_type sum = 0;
    memcpy(&sum, bytes.data() + n_chunk_check_sum, sizeof sum);

    return check_sum({ bytes.data() + n_chunk_data,
                       bytes.size() - n_chunk_data }) == sum;
  }

  auto stream_cipher::chunk_pack( //
      const size_t offset,        //
      cref_vbyte   bytes) -> vbyte {
    vbyte chunk(n_chunk_data + bytes.size());

    const uint64_t offset64 = offset;
    const uint64_t size     = bytes.size();
    const uint64_t sum      = check_sum(bytes);

    memcpy(chunk.data() + n_chunk_offset, &offset64, sizeof offset64);
    memcpy(chunk.data() + n_chunk_check_sum, &sum, sizeof sum);
    memcpy(chunk.data() + n_chunk_data, bytes.data(), bytes.size());

    return chunk;
  }

  auto stream_cipher::chunk_unpack(cref_vbyte bytes)
      -> tuple<vbyte, size_t> {
    if (bytes.size() < n_chunk_data) {
      verb_error(__FUNCTION__, "Invalid size.");
      return {};
    }

    uint64_t offset = 0;
    vbyte    chunk(bytes.size() - n_chunk_data);

    memcpy(&offset, bytes.data() + n_chunk_offset, sizeof offset);
    memcpy(chunk.data(), bytes.data() + n_chunk_data, chunk.size());

    return { chunk, static_cast<size_t>(offset) };
  }

  auto stream_cipher::chunk_encrypt(cref_vbyte bytes) -> vbyte {
    const auto block_count = (bytes.size() + (block_size - 1)) /
                             block_size;

    vbyte result(block_size * block_count);

    for (size_t i = 0; i < block_count; i++) {
      const auto offset = i * block_size;
      const auto size   = bytes.size() - offset;

      if (size < block_size) {
        uint8_t block[block_size] = { 0 };
        memcpy(block, bytes.data() + offset, size);

        if (!do_encrypt({ block, block_size },
                        { result.data() + offset, block_size }))
          return {};

      } else {
        if (!do_encrypt({ bytes.data() + offset, block_size },
                        { result.data() + offset, block_size }))
          return {};
      }

      m_enc_offset += block_size;
    }

    return result;
  }

  auto stream_cipher::rewind(const size_t offset) -> bool {
    if (offset < m_dec_offset) {
      if (!rewind_decryption())
        return false;

      m_dec_offset = 0;
    }

    if (offset > m_dec_offset) {
      if (!pass_decryption(offset - m_dec_offset))
        return false;

      m_dec_offset = offset;
    }

    return true;
  }

  auto stream_cipher::chunk_decrypt(const size_t offset,
                                    cref_vbyte   bytes) -> vbyte {
    if (!rewind(offset))
      return {};

    const auto block_count = (bytes.size() + (block_size - 1)) /
                             block_size;

    vbyte result(block_size * block_count);

    for (size_t i = 0; i < block_count; i++) {
      const auto offset = i * block_size;
      const auto size   = bytes.size() - offset;

      if (bytes.size() - offset < block_size) {
        verb_error(__FUNCTION__, "Invalid block size.");
        return {};
      }

      if (!do_decrypt({ bytes.data() + offset, block_size },
                      { result.data() + offset, block_size }))
        return {};

      m_dec_offset += block_size;
    }

    return result;
  }
}
