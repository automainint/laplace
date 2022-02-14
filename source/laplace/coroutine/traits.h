/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_COROUTINE_TRAITS_H
#define LAPLACE_COROUTINE_TRAITS_H

#include <concepts>
#include <utility>

namespace laplace::coroutine {
  template <typename promise_>
  concept promise_basic = requires(promise_ promise) {
    promise.initial_suspend();
    promise.final_suspend();
    promise.get_return_object();
    promise.unhandled_exception();
  };

  template <typename promise_>
  concept promise_value = promise_basic<promise_> &&
      requires(promise_ promise) {
    std::move(promise.m_value);
  };

  template <typename promise_>
  concept promise_returning = promise_basic<promise_> &&
      requires(promise_ promise) {
    promise.return_value(decltype(promise.m_value) {});
  };

  template <typename promise_>
  concept promise_void = promise_basic<promise_> &&
                         !promise_returning<promise_> &&
                         requires(promise_ promise) {
    promise.return_void();
  };

  template <typename promise_>
  concept promise_shared = promise_basic<promise_> &&
      requires(promise_ promise) {
    ++promise.m_ref_count;
    --promise.m_ref_count == 0;
  };

  template <typename promise_>
  concept promise_not_shared = promise_basic<promise_> &&
      !promise_shared<promise_>;

  template <typename promise_>
  concept promise_nested = promise_shared<promise_> &&
      requires(promise_ promise) {
    promise.m_nested;
  };
}

#endif
