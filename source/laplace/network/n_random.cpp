/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "random.h"

namespace laplace::network {
  random::random() noexcept {
    m_seed = generate<decltype(m_seed)>();
  }

  auto random::get_seed() const noexcept -> uint64_t {
    return m_seed;
  }

  auto random::generate_token() noexcept -> vbyte {
    return m_rng.generate(256);
  }
}
