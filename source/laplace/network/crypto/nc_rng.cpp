/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "rng.h"

#include "../../core/string.h"

namespace laplace::network::crypto {
  using std::move;

  rng::rng(rng &&other) noexcept :
      generator(move(other.generator)), m_removed(other.m_removed) {
    other.m_removed = true;
  }

  auto rng::operator=(rng &&other) noexcept -> rng & {
    _free();
    generator       = move(other.generator);
    m_removed       = other.m_removed;
    other.m_removed = true;
    return *this;
  }

  rng::rng() noexcept {
    if (auto _n = wc_InitRng(&generator); _n != 0)
      log(log_event::error, fmt("wc_InitRng failed (code: %d).", _n),
          __FUNCTION__);
  }

  rng::~rng() noexcept {
    _free();
  }

  auto rng::generate(sl::whole size) noexcept -> vbyte {
    auto block = vbyte(size);

    if (auto _n = wc_RNG_GenerateBlock(&generator, block.data(),
                                       block.size());
        _n != 0) {
      log(log_event::error,
          fmt("wc_RNG_GenerateBlock failed (code: %d).", _n),
          __FUNCTION__);
      return {};
    }

    return block;
  }

  void rng::_free() noexcept {
    if (m_removed)
      return;

    wc_FreeRng(&generator);
    m_removed = true;
  }
}
