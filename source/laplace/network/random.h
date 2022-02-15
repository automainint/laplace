/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_NETWORK_RANDOM_H
#define LAPLACE_NETWORK_RANDOM_H

#include "../core/serial.h"
#include "crypto/rng.h"

namespace laplace::network {
  class random {
  public:
    random(random const &)          = delete;
    random(random &&other) noexcept = default;
    random &operator=(random const &) = delete;
    random &operator=(random &&) = delete;

    random() noexcept;
    ~random() = default;

    [[nodiscard]] auto get_seed() const noexcept -> uint64_t;

    [[nodiscard]] auto generate_token() noexcept -> vbyte;

    template <typename type_>
    [[nodiscard]] auto generate() noexcept -> type_;

    template <typename type_>
    [[nodiscard]] auto generate(type_ begin, type_ end) noexcept
        -> type_;

  private:
    crypto::rng m_rng;
    uint64_t    m_seed = {};
  };
}

#include "random.impl.h"

#endif
