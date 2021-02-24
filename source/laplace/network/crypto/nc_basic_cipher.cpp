/*  laplace/network/crypto/nc_basic_cipher.h
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

#include "basic_cipher.h"

namespace laplace::network::crypto {
  using std::copy, std::string_view, std::string;

  void basic_cipher::set_remote_key(cref_vbyte key) {
    set_ready(setup_remote_key(key) && setup());
  }

  auto basic_cipher::encrypt(cref_vbyte bytes) -> vbyte {
    return { bytes.begin(), bytes.end() };
  }

  auto basic_cipher::decrypt(cref_vbyte bytes) -> vbyte {
    return { bytes.begin(), bytes.end() };
  }

  auto basic_cipher::is_ready() const noexcept -> bool {
    return m_is_ready;
  }

  auto basic_cipher::get_public_key() const noexcept -> cref_vbyte {
    return { m_public_key.data(), m_public_key_size };
  }

  auto basic_cipher::get_mutual_key() const noexcept -> cref_vbyte {
    return { m_mutual_key.data(), m_mutual_key_size };
  }

  void basic_cipher::verb_error(string_view sender,
                                string_view message) {
    if (m_is_verbose) {
      verb("[ %s ] %s", string(sender).c_str(),
           string(message).c_str());
    }
  }

  auto basic_cipher::setup_remote_key(cref_vbyte key) -> bool {
    return false;
  }

  auto basic_cipher::setup() -> bool {
    return false;
  }

  void basic_cipher::set_ready(bool is_ready) noexcept {
    m_is_ready = is_ready;
  }

  void basic_cipher::set_private_key(cref_vbyte key) noexcept {
    if (key.size() > m_private_key.size()) {
      error(__FUNCTION__, "Invalid key size.");
      return;
    }

    copy(key.begin(), key.end(), m_private_key.begin());
    m_private_key_size = key.size();
  }

  void basic_cipher::set_public_key(cref_vbyte key) noexcept {
    if (key.size() > m_public_key.size()) {
      error(__FUNCTION__, "Invalid key size.");
      return;
    }

    copy(key.begin(), key.end(), m_public_key.begin());
    m_public_key_size = key.size();
  }

  void basic_cipher::set_mutual_key(cref_vbyte key) noexcept {
    if (key.size() > m_mutual_key.size()) {
      error(__FUNCTION__, "Invalid key size.");
      return;
    }

    copy(key.begin(), key.end(), m_mutual_key.begin());
    m_mutual_key_size = key.size();
  }

  auto basic_cipher::get_private_key() const noexcept -> cref_vbyte {
    return { m_private_key.data(), m_private_key_size };
  }
}
