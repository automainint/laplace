/*  laplace/network/n_transfer.cpp
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

#include "../core/utils.h"
#include "transfer.h"

namespace laplace::network {
  using std::min, std::unique_ptr, crypto::basic_cipher, std::span,
      std::vector;

  void transfer::set_cipher(unique_ptr<basic_cipher> cipher) {
    m_cipher = std::move(cipher);
    m_cipher->set_verbose(true);
  }

  void transfer::set_remote_key(cref_vbyte key) {
    if (m_cipher) {
      m_cipher->set_remote_key(key);
    } else {
      error(__FUNCTION__, "No cipher.");
    }
  }

  auto transfer::pack(span<const cref_vbyte> data) -> vbyte {
    return pack_internal(data, mark_plain);
  }

  auto transfer::unpack(cref_vbyte data) -> vector<vbyte> {
    m_loss_count = 0;
    return unpack_internal(data, mark_plain);
  }

  auto transfer::encode(span<const cref_vbyte> data) -> vbyte {
    if (is_encrypted()) {
      return m_cipher->encrypt(pack_internal(data, mark_encrypted));
    }

    return pack_internal(data, mark_plain);
  }

  auto transfer::decode(cref_vbyte data) -> vector<vbyte> {
    if (is_encrypted()) {
      m_loss_count = m_cipher->get_loss_count();
      return unpack_internal(m_cipher->decrypt(data), mark_encrypted);
    }

    m_loss_count = 0;
    return unpack_internal(data, mark_plain);
  }

  auto transfer::get_public_key() const noexcept -> cref_vbyte {
    if (m_cipher) {
      return m_cipher->get_public_key();
    }

    error(__FUNCTION__, "No cipher.");
    return {};
  }

  auto transfer::get_mutual_key() const noexcept -> cref_vbyte {
    if (m_cipher) {
      return m_cipher->get_mutual_key();
    }

    error(__FUNCTION__, "No cipher.");
    return {};
  }

  auto transfer::is_encrypted() const noexcept -> bool {
    return m_cipher && m_cipher->is_ready();
  }

  auto transfer::get_loss_count() const noexcept -> size_t {
    return m_loss_count;
  }

  auto transfer::check_sum(cref_vbyte data) -> uint64_t {
    uint64_t sum = 0;

    for (size_t i = 0; i < data.size(); i += sizeof sum) {
      uint64_t z = 0;

      memcpy(              //
          &z,              //
          data.data() + i, //
          min(sizeof z, data.size() - i));

      sum ^= z;
    }

    return sum;
  }

  auto transfer::pack_internal(    //
      span<const cref_vbyte> data, //
      const uint16_t         mark) -> vbyte {

    size_t size = 0;
    for (size_t i = 0; i < data.size(); i++) {
      size += n_data + data[i].size();
    }

    auto buf = vbyte {};
    buf.reserve(size);

    for (size_t i = 0; i < data.size(); i++) {
      const size_t offset = buf.size();
      buf.resize(offset + n_data + data[i].size());

      const uint64_t sum = check_sum(data[i]);
      const uint64_t n   = data[i].size();

      wr(buf, offset + n_mark, mark);
      wr(buf, offset + n_sum, sum);
      wr(buf, offset + n_size, n);

      memcpy(                           //
          buf.data() + offset + n_data, //
          data[i].data(),               //
          data[i].size());
    }

    return buf;
  }

  auto transfer::unpack_internal( //
      cref_vbyte     data,        //
      const uint16_t mark) -> vector<vbyte> {

    vector<vbyte> buf;
    size_t        offset = 0;

    while (offset < data.size()) {
      const auto size = scan(
          { data.begin() + static_cast<ptrdiff_t>(offset), data.end() },
          mark);

      if (size > 0) {
        offset += n_data;

        buf.emplace_back(vbyte {
            data.begin() + static_cast<ptrdiff_t>(offset),
            data.begin() + static_cast<ptrdiff_t>(offset + size) });

        offset += size;

      } else {
        m_loss_count++;
        offset++;
      }
    }

    return buf;
  }

  auto transfer::scan( //
      cref_vbyte data, //
      uint16_t   mark) const noexcept -> size_t {

    if (rd<uint16_t>(data, n_mark) != mark)
      return 0;

    const auto sum  = rd<uint64_t>(data, n_sum);
    const auto size = static_cast<size_t>(rd<uint64_t>(data, n_size));

    if (size + n_data > data.size()) {
      return 0;
    }

    if (sum != check_sum({ data.data() + n_data, size })) {
      verb("Transfer: Wrong check sum.");
      return 0;
    }

    return size;
  }
}
