/*  laplace/network/crypto/deffiehellman.h
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

#ifndef laplace_network_crypto_deffiehellman_h
#define laplace_network_crypto_deffiehellman_h

#include "basic_cipher.h"

#include <wolfssl/options.h>

#include <wolfssl/wolfcrypt/dh.h>
#include <wolfssl/wolfcrypt/random.h>

namespace laplace::network::crypto {
  class deffiehellman : virtual public basic_cipher {
  public:
    static constexpr sl::index buffer_size = 0x1000;

    deffiehellman(const sl::index key_size);
    ~deffiehellman() override;

  protected:
    auto setup_remote_key(span_cbyte key) -> bool override;

  private:
    bool   m_is_ok  = false;
    WC_RNG m_random = {};
    DhKey  m_key    = {};
  };
}

#endif
