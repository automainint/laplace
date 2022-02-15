/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_NETWORK_CRYPTO_ECC_H
#define LAPLACE_NETWORK_CRYPTO_ECC_H

#include "basic_cipher.h"
#include "rng.h"

#include <wolfssl/wolfcrypt/ecc.h>

namespace laplace::network::crypto {
  class ecc : virtual public basic_cipher {
  public:
    static const sl::whole buffer_size;

    ecc(sl::whole key_size);
    ~ecc() override;

  protected:
    auto setup_remote_key(span_cbyte key) -> bool override;

  private:
    rng     m_rng;
    bool    m_is_ok   = false;
    ecc_key m_private = {};
    ecc_key m_remote  = {};
  };
}

#endif
