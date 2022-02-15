/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_NETWORK_RANDOM_IMPL_H
#define LAPLACE_NETWORK_RANDOM_IMPL_H

namespace laplace::network {
  template <typename type_>
  inline auto random::generate() noexcept -> type_ {
    return serial::rd<type_>(m_rng.generate(sizeof(type_)), 0);
  }

  template <typename type_>
  inline auto random::generate(type_ begin, type_ end) noexcept -> type_ {
    if (end <= begin) {
      error_("Invalid range.", __FUNCTION__);
      return {};
    }

    return begin + (generate<type_>() % (end - begin));
  }
}

#endif
