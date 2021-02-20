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
    auto status = wc_InitRng(&m_random);

    if (status != 0) {
      error(__FUNCTION__, "wc_InitRng failed.");
      m_is_ok = false;
      return;
    }

    status = wc_InitDhKey(&m_key);

    if (status != 0) {
      error(__FUNCTION__, "wc_InitDhKey failed.");
      m_is_ok = false;
      return;
    }

    status = wc_DhSetKey(
        &m_key, prime, sizeof prime, generator, sizeof generator);

    if (status != 0) {
      error(__FUNCTION__, "wc_DhSetKey failed.");
      m_is_ok = false;
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

  auto dh_rabbit::operator=(dh_rabbit &&r) noexcept -> dh_rabbit & {

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
    return { m_public.data(), m_public_size };
  }

  auto dh_rabbit::get_mutual_key() const -> cref_vbyte {
    return { m_mutual.data(), m_mutual_size };
  }

  void dh_rabbit::set_remote_key(cref_vbyte key) {
    if (!m_is_ok) {
      return;
    }

    m_is_ready    = false;
    m_mutual_size = key_size;

    auto status = wc_DhAgree(&m_key, m_mutual.data(), &m_mutual_size,
                             m_private.data(), m_private_size,
                             key.data(), key.size());

    if (status != 0) {
      error(__FUNCTION__, "wc_DhAgree failed.");
      return;
    }

    status = wc_RabbitSetKey( //
        &m_encrypt,           //
        m_mutual.data(),      //
        m_mutual.data() + 16);

    if (status != 0) {
      error(__FUNCTION__, "wc_RabbitSetKey failed.");
      return;
    }

    status = wc_RabbitSetKey( //
        &m_decrypt,           //
        m_mutual.data(),      //
        m_mutual.data() + 16);

    if (status != 0) {
      error(__FUNCTION__, "wc_RabbitSetKey failed.");
      return;
    }

    m_is_ready = true;
  }

  auto dh_rabbit::pass(const size_t offset) -> bool {
    if (offset > loss_limit) {
      return false;
    }

    vbyte buf(offset);
    vbyte dst(offset);

    const auto status = wc_RabbitProcess( //
        &m_decrypt,                       //
        buf.data(),                       //
        dst.data(),                       //
        offset);

    if (status != 0) {
      error(__FUNCTION__, "wc_RabbitProcess failed.");
      return false;
    }

    m_dec_offset += offset;
    return true;
  }

  auto dh_rabbit::check_sum(cref_vbyte bytes) -> uint16_t {
    uint16_t sum = 0;
    size_t   i   = 0;
    for (; i + 1 < bytes.size(); i += 2) {
      sum ^= rd<uint16_t>(bytes, i);
    }
    if (i < bytes.size())
      sum ^= bytes[i];
    return sum;
  }

  auto dh_rabbit::check(cref_vbyte bytes) -> bool {
    if (bytes.size() < n_check_sum) {
      return false;
    }

    const auto size = bytes.size() - n_check_sum;

    if (rd<sum_type>(bytes, size + n_check_null) != 0) {
      return false;
    }

    const auto sum = check_sum({ bytes.data(), size });

    return rd<sum_type>(bytes, size) == sum;
  }

  auto dh_rabbit::encrypt(cref_vbyte bytes) -> vbyte {
    if (!is_ready() || bytes.empty()) {
      return vbyte { bytes.begin(), bytes.end() };
    }

    vbyte buf(bytes.size() + n_check_sum);

    std::copy(bytes.begin(), bytes.end(), buf.begin());
    wr<sum_type>(buf, bytes.size(), check_sum(bytes));
    wr<sum_type>(buf, bytes.size() + n_check_null, 0u);

    const auto size  = buf.size() + n_header;
    const auto delta = buf.size();

    vbyte result(size + n_check_sum);

    const auto status = wc_RabbitProcess( //
        &m_encrypt,                       //
        result.data() + n_header,         //
        buf.data(),                       //
        delta);

    if (status != 0) {
      error(__FUNCTION__, "wc_RabbitProcess failed.");
      return {};
    }

    wr<uint64_t>(result, n_offset, m_enc_offset);
    wr<uint64_t>(result, n_size, delta);
    m_enc_offset += delta;

    wr<sum_type>(result, size, check_sum({ result.data(), size }));
    wr<sum_type>(result, size + n_check_null, 0u);

    return result;
  }

  auto dh_rabbit::decrypt(cref_vbyte bytes) -> vbyte {
    if (!is_ready() || bytes.empty()) {
      return vbyte { bytes.begin(), bytes.end() };
    }

    vbyte  result;
    size_t i = 0;

    while (i < bytes.size()) {
      if (bytes.size() < i + n_overhead) {
        break;
      }

      const auto size      = as_index(rd<uint64_t>(bytes, n_size));
      const auto full_size = size + n_overhead;

      if (i + full_size > bytes.size()) {
        break;
      }

      const auto buf = decrypt_chunk({ bytes.data() + i, full_size });

      if (buf.empty()) {
        break;
      }

      result.insert(result.end(), buf.begin(), buf.end());
      i += full_size;
    }

    return result;
  }

  auto dh_rabbit::decrypt_chunk(cref_vbyte bytes) -> vbyte {
    if (bytes.size() < n_overhead + n_check_sum) {
      return {};
    }

    if (!check(bytes)) {
      return {};
    }

    const auto offset = as_index(rd<uint64_t>(bytes, n_offset));

    if (offset == -1) {
      return {};
    }

    if (offset > m_dec_offset) {
      if (!pass(offset - m_dec_offset))
        return {};
    } else if (offset < m_dec_offset) {
      return {};
    }

    vbyte      buf(bytes.size() - n_overhead);
    const auto delta = buf.size();

    const auto status = wc_RabbitProcess( //
        &m_decrypt,                       //
        buf.data(),                       //
        bytes.data() + n_header,          //
        delta);

    if (status != 0) {
      error(__FUNCTION__, "wc_RabbitProcess failed.");
      return {};
    }

    m_dec_offset += delta;

    if (!check(buf)) {
      verb("Rabbit: WRONG CHECK SUM");
      return {};
    }

    buf.resize(buf.size() - n_check_sum);
    return buf;
  }

  auto dh_rabbit::is_ready() -> bool {
    return m_is_ok && m_is_ready;
  }
}
