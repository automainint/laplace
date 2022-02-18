/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "basic_cipher.h"

#include "../../core/string.h"

namespace laplace::network::crypto {
  using std::copy, std::string_view, std::string;

  void basic_cipher::set_remote_key(span_cbyte key) {
    set_ready(setup_remote_key(key) && setup());
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

  void basic_cipher::set_ready(bool is_ready) noexcept {
    m_is_ready = is_ready;
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
      log(log_event::error, "Invalid byte count.", __FUNCTION__);
      return;
    }

    m_loss_count += count;
  }
}
