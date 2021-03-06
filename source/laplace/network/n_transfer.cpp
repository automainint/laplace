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

#include "transfer.h"

namespace laplace::network {
  using std::min, std::unique_ptr, crypto::basic_cipher;

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

  auto transfer::pack(cref_vbyte data) -> vbyte {
    auto buf = vbyte(n_data + data.size());

    const uint64_t size = data.size();
    const auto     sum  = check_sum(data);

    memcpy(buf.data() + n_size, &size, sizeof size);
    memcpy(buf.data() + n_check_sum, &sum, sizeof sum);
    memcpy(buf.data() + n_data, data.data(), data.size());

    return buf;
  }

  auto transfer::unpack(cref_vbyte data) -> vbyte {
    if (data.empty()) {
      return {};
    }

    if (data.size() < n_data) {
      verb("Transfer: Invalid data.");
      return {};
    }

    uint64_t size;
    uint64_t sum;

    memcpy(&size, data.data() + n_size, sizeof size);
    memcpy(&sum, data.data() + n_check_sum, sizeof sum);

    if (n_data + size > data.size()) {
      verb("Transfer: Invalid data size.");
      return {};
    }

    if (check_sum({ data.data() + n_data, size }) != sum) {
      verb("Transfer: WRONG CHECK SUM");
      return {};
    }

    return { data.begin() + n_data, data.begin() + n_data + size };
  }

  auto transfer::encode(cref_vbyte data) -> vbyte {
    if (m_cipher && m_cipher->is_ready()) {
      return m_cipher->encrypt(pack(data));
    }

    return pack(data);
  }

  auto transfer::decode(cref_vbyte data) -> vbyte {
    if (m_cipher && m_cipher->is_ready()) {
      return unpack(m_cipher->decrypt(data));
    }

    return unpack(data);
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
}
