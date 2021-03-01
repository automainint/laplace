/*  laplace/network/crypto/nc_rabbit.cpp
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

#include "rabbit.h"

namespace laplace::network::crypto {
  using std::span;

  auto rabbit::setup() -> bool {
    const auto key = get_mutual_key();

    if (key.size() < key_size) {
      error(__FUNCTION__, "Invalid key.");
      return false;
    }

    const auto status = wc_RabbitSetKey( //
        &m_origin,                       //
        key.data(),                      //
        key.data() + key_extra);

    if (status != 0) {
      error(__FUNCTION__, "wc_RabbitSetKey failed.");
      return false;
    }

    m_encrypt = m_origin;
    m_decrypt = m_origin;

    return true;
  }

  auto rabbit::do_encrypt(cref_vbyte src, span<uint8_t> dst) -> bool {
    if (dst.size() < src.size()) {
      error(__FUNCTION__, "Invalid destination.");
      return false;
    }

    const auto status = wc_RabbitProcess(
        &m_encrypt, dst.data(), src.data(), src.size());

    if (status != 0) {
      error(__FUNCTION__, "wc_RabbitProcess failed.");
      return false;
    }

    return true;
  }

  auto rabbit::do_decrypt(cref_vbyte src, span<uint8_t> dst) -> bool {
    if (dst.size() < src.size()) {
      error(__FUNCTION__, "Invalid destination.");
      return false;
    }

    const auto status = wc_RabbitProcess(
        &m_decrypt, dst.data(), src.data(), src.size());

    if (status != 0) {
      error(__FUNCTION__, "wc_RabbitProcess failed.");
      return false;
    }

    return true;
  }

  auto rabbit::rewind_decryption() -> bool {
    m_decrypt = m_origin;
    return true;
  }
}