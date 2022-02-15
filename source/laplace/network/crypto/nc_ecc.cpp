/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "ecc.h"

#include "../../core/string.h"

namespace laplace::network::crypto {
  const sl::whole ecc::buffer_size = 0x1000;

  ecc::ecc(sl::whole const key_size) {
    if (auto _n = wc_ecc_init(&m_private); _n != 0) {
      error_(fmt("wc_ecc_init failed (code: %d).", _n), __FUNCTION__);
      return;
    }

    if (auto _n = wc_ecc_init(&m_remote); _n != 0) {
      error_(fmt("wc_ecc_init failed (code: %d).", _n), __FUNCTION__);
      return;
    }

#ifdef ECC_TIMING_RESISTANT
    if (auto _n = wc_ecc_set_rng(&m_private, &m_rng.generator);
        _n != 0) {
      error_(fmt("wc_ecc_set_rng failed (code: %d).", _n),
             __FUNCTION__);
      return;
    }
#endif

    if (auto _n = wc_ecc_make_key(
            &m_rng.generator, static_cast<int>(key_size), &m_private);
        _n != 0) {
      error_(fmt("wc_ecc_make_key failed (code: %d).", _n),
             __FUNCTION__);
      return;
    }

    auto buf  = std::array<uint8_t, buffer_size> {};
    auto size = static_cast<uint32_t>(buffer_size);

    if (auto _n = wc_ecc_export_x963(&m_private, buf.data(), &size);
        _n != 0) {
      error_(fmt("wc_ecc_export_x963 failed (code: %d).", _n),
             __FUNCTION__);
      return;
    }

    if (buf.size() < size) {
      error_("Invalid public key size.", __FUNCTION__);
      return;
    }

    set_public_key({ buf.data(), size });

    m_is_ok = true;
  }

  ecc::~ecc() {
    wc_ecc_free(&m_private);
    wc_ecc_free(&m_remote);
  }

  auto ecc::setup_remote_key(span_cbyte key) -> bool {
    if (!m_is_ok)
      return false;

    if (auto _n = wc_ecc_import_x963(
            key.data(), static_cast<uint32_t>(key.size()), &m_remote);
        _n != 0) {
      error_(fmt("wc_ecc_import_x963 failed (code: %d).", _n),
             __FUNCTION__);
      return false;
    }

    auto buf  = std::array<uint8_t, buffer_size> {};
    auto size = static_cast<uint32_t>(buffer_size);

    if (auto _n = wc_ecc_shared_secret(&m_private, &m_remote,
                                       buf.data(), &size);
        _n != 0) {
      error_(fmt("wc_ecc_shared_secret failed (code: %d).", _n),
             __FUNCTION__);
      return false;
    }

    if (buf.size() < size) {
      error_("Invalid mutual key size.", __FUNCTION__);
      return false;
    }

    set_mutual_key({ buf.data(), size });

    return true;
  }
}
