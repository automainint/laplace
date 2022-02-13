/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_stem_server_impl_h
#define laplace_stem_server_impl_h

namespace laplace::stem {
  template <typename factory>
  inline void server::make_factory() noexcept { }

  template <typename impact, typename... args_>
  inline void server::emit(args_... args) noexcept { }
}

#endif