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

#include "../core/defs.h"
#include <random>

namespace laplace::network {
  class random {
  public:
    random() noexcept;
    ~random() = default;

    [[nodiscard]] auto get_seed() const noexcept -> uint64_t;
    [[nodiscard]] auto generate_token() noexcept -> vbyte;

  private:
    std::mt19937_64 m_engine;
    uint64_t        m_seed = {};
  };
}

#endif
