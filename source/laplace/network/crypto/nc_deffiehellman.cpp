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

#include "deffiehellman.h"
#include "prime.h"

namespace laplace::network::crypto {
  using std::max;

  deffiehellman::deffiehellman(size_t key_size) {
    auto status = wc_InitRng(&m_random);

    if (status != 0) {
      error_("wc_InitRng failed.", __FUNCTION__);
      return;
    }

    status = wc_InitDhKey(&m_key);

    if (status != 0) {
      error_("wc_InitDhKey failed.", __FUNCTION__);
      return;
    }

    status = wc_DhSetKey(    //
        &m_key,              //
        prime, sizeof prime, //
        generator, sizeof generator);

    if (status != 0) {
      error_("wc_DhSetKey failed.", __FUNCTION__);
      return;
    }

    vbyte priv_key(key_size);
    vbyte publ_key(key_size);

    uint32_t priv_size = static_cast<uint32_t>(key_size);
    uint32_t publ_size = static_cast<uint32_t>(key_size);

    status = wc_DhGenerateKeyPair(   //
        &m_key, &m_random,           //
        priv_key.data(), &priv_size, //
        publ_key.data(), &publ_size);

    if (status != 0) {
      error_("wc_DhGenerateKeyPair failed.", __FUNCTION__);
      return;
    }

    set_private_key({ priv_key.data(), priv_size });
    set_public_key({ publ_key.data(), publ_size });

    m_is_ok = true;
  }

  auto deffiehellman::setup_remote_key(span_cbyte key) -> bool {
    if (m_is_ok) {
      const auto priv_key = get_private_key();

      uint32_t mutual_size = static_cast<uint32_t>(
          max(key.size(), priv_key.size()));

      vbyte mutual_key(mutual_size * 2);

      auto status = wc_DhAgree(                   //
          &m_key,                                 //
          mutual_key.data(),                      //
          &mutual_size,                           //
          priv_key.data(),                        //
          static_cast<uint32_t>(priv_key.size()), //
          key.data(),                             //
          static_cast<uint32_t>(key.size()));

      if (status != 0) {
        error_("wc_DhAgree failed.", __FUNCTION__);
        return false;
      }

      if (mutual_key.size() < mutual_size) {
        error_("Invalid mutual key size.", __FUNCTION__);
        return false;
      }

      set_mutual_key({ mutual_key.data(), mutual_size });
    }

    return m_is_ok;
  }
}
