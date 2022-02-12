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
    auto dev = std::random_device {};
    m_engine.seed(dev());
    m_seed = m_engine();
  }

  auto random::get_seed() const noexcept -> uint64_t {
    return m_seed;
  }

  auto random::generate_token() noexcept -> vbyte {
    auto token    = vbyte(256);
    auto get_byte = std::uniform_int_distribution<int>(0, 255);
    for (auto &byte : token)
      byte = static_cast<uint8_t>(get_byte(m_engine));
    return token;
  }
}
