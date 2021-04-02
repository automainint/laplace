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

#ifndef WC_NO_HARDEN
#define WC_NO_HARDEN 1
#endif

#include <wolfssl/wolfcrypt/dh.h>
#include <wolfssl/wolfcrypt/random.h>

#ifdef min
#  undef min
#endif

#ifdef max
#  undef max
#endif

namespace laplace::network::crypto {
  class deffiehellman : virtual public basic_cipher {
  public:
    deffiehellman(size_t key_size);
    ~deffiehellman() override = default;

  protected:
    auto setup_remote_key(span_cbyte key) -> bool override;

  private:
    bool   m_is_ok  = false;
    WC_RNG m_random = { 0 };
    DhKey  m_key    = { 0 };
  };
}

#endif
