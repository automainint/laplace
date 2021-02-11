/*  laplace/network/crypto/nc_dh_rabbit.cpp
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

#include "../../core/utils.h"
#include "dh_rabbit.h"
#include "prime.h"

namespace laplace::network::crypto {
  void dh_rabbit::init() {
    wc_InitRng(&m_random);
    wc_InitDhKey(&m_key);

    auto status = wc_DhSetKey(&m_key, prime, sizeof prime,
                              generator, sizeof generator);

    if (status != 0) {
      error(__FUNCTION__, "wc_DhSetKey failed.");
      m_is_ok = false;
      return;
    }
  }

  dh_rabbit::dh_rabbit() {
    init();

    if (m_is_ok) {
      auto status = wc_DhGenerateKeyPair(
          &m_key, &m_random, m_private.data(), &m_private_size,
          m_public.data(), &m_public_size);

      if (status != 0) {
        error(__FUNCTION__, "wc_DhGenerateKeyPair failed.");
        m_is_ok = false;
        return;
      }
    }
  }

  dh_rabbit::dh_rabbit(dh_rabbit &&r) noexcept {
    init();

    if (m_is_ok) {
      m_private_size = std::move(r.m_private_size);
      m_public_size  = std::move(r.m_public_size);
      m_mutual_size  = std::move(r.m_mutual_size);
      m_private      = std::move(r.m_private);
      m_public       = std::move(r.m_public);
      m_mutual       = std::move(r.m_mutual);

      m_encrypt = std::move(r.m_encrypt);
      m_decrypt = std::move(r.m_decrypt);
    }
  }

  auto dh_rabbit::operator=(dh_rabbit &&r) noexcept
      -> dh_rabbit & {

    m_private_size = std::move(r.m_private_size);
    m_public_size  = std::move(r.m_public_size);
    m_mutual_size  = std::move(r.m_mutual_size);
    m_private      = std::move(r.m_private);
    m_public       = std::move(r.m_public);
    m_mutual       = std::move(r.m_mutual);

    m_encrypt = std::move(r.m_encrypt);
    m_decrypt = std::move(r.m_decrypt);

    return *this;
  }

  dh_rabbit::~dh_rabbit() {
    wc_FreeRng(&m_random);
    wc_FreeDhKey(&m_key);
  }

  auto dh_rabbit::get_public_key() const -> cref_vbyte {
    return m_public;
  }

  auto dh_rabbit::get_mutual_key() const -> cref_vbyte {
    return m_mutual;
  }

  void dh_rabbit::set_remote_key(cref_vbyte key) {
    if (!m_is_ok)
      return;

    m_is_ready    = false;
    m_mutual_size = key_size;

    auto status = wc_DhAgree(&m_key, m_mutual.data(),
                             &m_mutual_size, m_private.data(),
                             m_private_size, key.data(),
                             key.size());

    if (status != 0) {
      error(__FUNCTION__, "wc_DhAgree failed.");
      return;
    }

    status = wc_RabbitSetKey(
        &m_encrypt, m_mutual.data(), m_mutual.data() + 16);

    if (status != 0) {
      error(__FUNCTION__, "wc_RabbitSetKey failed.");
      return;
    }

    m_decrypt = m_encrypt;

    m_is_ready = true;
  }

  auto dh_rabbit::encrypt(cref_vbyte bytes) -> vbyte {
    if (!is_ready() || bytes.empty()) {
      return vbyte(bytes.begin(), bytes.end());
    }

    vbyte result(bytes.size());

    auto status = wc_RabbitProcess(
        &m_encrypt, result.data(), bytes.data(), bytes.size());

    if (status != 0) {
      error(__FUNCTION__, "wc_RabbitProcess failed.");
      return {};
    }

    return result;
  }

  auto dh_rabbit::decrypt(cref_vbyte bytes) -> vbyte {
    if (!is_ready() || bytes.empty()) {
      return vbyte(bytes.begin(), bytes.end());
    }

    vbyte result(bytes.size());

    auto status = wc_RabbitProcess(
        &m_decrypt, result.data(), bytes.data(), bytes.size());

    if (status != 0) {
      error(__FUNCTION__, "wc_RabbitProcess failed.");
      return {};
    }

    return result;
  }

  auto dh_rabbit::is_ready() -> bool {
    return m_is_ok && m_is_ready;
  }
}
