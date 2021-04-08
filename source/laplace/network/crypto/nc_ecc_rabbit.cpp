/*  laplace/network/crypto/nc_ecc_rabbit.cpp
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

#include "ecc_rabbit.h"

namespace laplace::network::crypto {
  ecc_rabbit::ecc_rabbit() : ecc(key_size) { }
}
