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

#include "dh_rabbit.h"

namespace laplace::network::crypto {
  dh_rabbit::dh_rabbit() : deffiehellman(key_size) { }
}
