/*  laplace/network/crypto/nc_deffiehellman.cpp
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
#include "deffiehellman.h"
#include "prime.h"

namespace laplace::network::crypto {
  using std::max;

  deffiehellman::deffiehellman(const sl::index key_size) {
    auto status = wc_InitRng(&m_random);

    if (status != 0) {
      error_(
          fmt("wc_InitRng failed (code: %d).", status), __FUNCTION__);
      return;
    }

    status = wc_InitDhKey(&m_key);

    if (status != 0) {
      error_(fmt("wc_InitDhKey failed (code: %d).", status),
             __FUNCTION__);
      return;
    }

    status = wc_DhSetKey(
        &m_key, prime, sizeof prime, generator, sizeof generator);

    if (status != 0) {
      error_(fmt("wc_DhSetKey failed (code: %d).", status),
             __FUNCTION__);
      return;
    }

    auto priv_key = vbyte(key_size);
    auto publ_key = vbyte(key_size);

    auto priv_size = static_cast<uint32_t>(key_size);
    auto publ_size = static_cast<uint32_t>(key_size);

    status = wc_DhGenerateKeyPair(&m_key, &m_random, priv_key.data(),
                                  &priv_size, publ_key.data(),
                                  &publ_size);

    if (status != 0) {
      error_(fmt("wc_DhGenerateKeyPair failed (code: %d).", status),
             __FUNCTION__);
      return;
    }

    set_private_key({ priv_key.data(), priv_size });
    set_public_key({ publ_key.data(), publ_size });

    m_is_ok = true;
  }

  deffiehellman::~deffiehellman() {
    wc_FreeDhKey(&m_key);
    wc_FreeRng(&m_random);
  }

  auto deffiehellman::setup_remote_key(span_cbyte key) -> bool {
    if (!m_is_ok)
      return false;

    const auto priv_key = get_private_key();

    auto mutual_key  = vbyte(buffer_size);
    auto mutual_size = static_cast<uint32_t>(buffer_size);

    auto status = wc_DhAgree(
        &m_key, mutual_key.data(), &mutual_size, priv_key.data(),
        static_cast<uint32_t>(priv_key.size()), key.data(),
        static_cast<uint32_t>(key.size()));

    if (status != 0) {
      error_(
          fmt("wc_DhAgree failed (code: %d).", status), __FUNCTION__);
      return false;
    }

    if (mutual_key.size() < mutual_size) {
      error_("Invalid mutual key size.", __FUNCTION__);
      return false;
    }

    set_mutual_key({ mutual_key.data(), mutual_size });

    return true;
  }
}
