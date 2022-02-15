/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_NETWORK_CRYPTO_RNG_H
#define LAPLACE_NETWORK_CRYPTO_RNG_H

#include "../../core/defs.h"

#include <wolfssl/options.h>

#include <wolfssl/wolfcrypt/random.h>

namespace laplace::network::crypto {
  struct rng {
    WC_RNG generator = {};

    rng(rng const &) = delete;
    rng &operator=(rng const &) = delete;
    rng &operator=(rng &&) = delete;

    rng() noexcept;
    rng(rng &&other) noexcept;
    ~rng() noexcept;

    [[nodiscard]] auto generate(sl::whole size) noexcept -> vbyte;

  private:
    bool m_removed = false;
  };
}

#endif
