/*  laplace/network/crypto/ecc_rabbit.h
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

#ifndef laplace_network_crypto_ecc_rabbit_h
#define laplace_network_crypto_ecc_rabbit_h

#include "ecc.h"
#include "rabbit.h"

namespace laplace::network::crypto {
  class ecc_rabbit : public ecc, public rabbit {
  public:
    static const sl::whole key_size;

    ecc_rabbit();
    ~ecc_rabbit() override = default;
  };
}

#endif
