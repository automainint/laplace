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
    log_handler log = get_global_log();

    WC_RNG generator = {};

    rng(rng const &) = delete;
    rng &operator=(rng const &) = delete;

    rng(rng &&other) noexcept;
    auto operator=(rng &&other) noexcept -> rng &;

    rng() noexcept;
    ~rng() noexcept;

    [[nodiscard]] auto generate(sl::whole size) noexcept -> vbyte;

  private:
    void _free() noexcept;

    bool m_removed = false;
  };
}

#endif