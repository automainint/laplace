/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_NETWORK_SERVER_IMPL_H
#define LAPLACE_NETWORK_SERVER_IMPL_H

namespace laplace::network {
  inline auto server::await_listen(
      std::span<endpoint_info const> endpoints) noexcept
      -> coroutine::task<> {
    return _await(this->listen(endpoints));
  }

  inline auto server::await_listen(endpoint_info const &info)
      -> coroutine::task<> {
    return await_listen(sl::vector<endpoint_info> { info });
  }

  inline auto server::await_connect(connect_info const &info)
      -> coroutine::task<> {
    return _await(this->connect(info));
  }

  inline auto server::_await(auto awaitable) -> coroutine::task<> {
    awaitable.resume();
    return awaitable;
  }
}

#endif
