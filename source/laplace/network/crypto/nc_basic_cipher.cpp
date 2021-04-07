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

#include "../../core/string.h"
#include "basic_cipher.h"

namespace laplace::network::crypto {
  using std::copy, std::string_view, std::string;

  void basic_cipher::set_remote_key(span_cbyte key) {
    set_ready(setup_remote_key(key) && setup());
  }

  void basic_cipher::set_verbose(bool is_verbose) noexcept {
    m_is_verbose = is_verbose;
  }

  auto basic_cipher::encrypt(span_cbyte bytes) -> vbyte {
    return { bytes.begin(), bytes.end() };
  }

  auto basic_cipher::decrypt(span_cbyte bytes) -> vbyte {
    return { bytes.begin(), bytes.end() };
  }

  auto basic_cipher::is_ready() const noexcept -> bool {
    return m_is_ready;
  }

  auto basic_cipher::get_public_key() const noexcept -> span_cbyte {
    return { m_public_key.data(), m_public_key.size() };
  }

  auto basic_cipher::get_mutual_key() const noexcept -> span_cbyte {
    return { m_mutual_key.data(), m_mutual_key.size() };
  }

  auto basic_cipher::get_loss_count() const noexcept -> sl::whole {
    return m_loss_count;
  }

  void basic_cipher::verb_error(string_view sender,
                                string_view message) const {
    if (m_is_verbose) {
      verb(fmt(
          "%s: %s", string(sender).c_str(), string(message).c_str()));
    }
  }

  auto basic_cipher::setup_remote_key(span_cbyte key) -> bool {
    error_("Not implemented.", __FUNCTION__);
    return false;
  }

  auto basic_cipher::setup() -> bool {
    error_("Not implemented.", __FUNCTION__);
    return false;
  }

  void basic_cipher::set_ready(bool is_ready) noexcept {
    m_is_ready = is_ready;
  }

  void basic_cipher::set_private_key(span_cbyte key) noexcept {
    m_private_key = vbyte { key.begin(), key.end() };
  }

  void basic_cipher::set_public_key(span_cbyte key) noexcept {
    m_public_key = vbyte { key.begin(), key.end() };
  }

  void basic_cipher::set_mutual_key(span_cbyte key) noexcept {
    m_mutual_key = vbyte { key.begin(), key.end() };
  }

  void basic_cipher::reset_loss_count() noexcept {
    m_loss_count = 0;
  }

  void basic_cipher::add_bytes_lost(sl::whole count) noexcept {
    if (count < 0) {
      error_("Invalid byte count.", __FUNCTION__);
      return;
    }

    m_loss_count += count;
  }

  auto basic_cipher::get_private_key() const noexcept -> span_cbyte {
    return { m_private_key.data(), m_private_key.size() };
  }
}
