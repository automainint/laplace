/*  laplace/network/crypto/dh_rabbit.h
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

#ifndef laplace_network_crypto_dh_rabbit_h
#define laplace_network_crypto_dh_rabbit_h

#include "deffiehellman.h"
#include "rabbit.h"

namespace laplace::network::crypto {
  class dh_rabbit : public deffiehellman, public rabbit {
  public:
    static constexpr sl::whole key_size = 32;

    dh_rabbit();
    ~dh_rabbit() override = default;
  };
}

#endif
