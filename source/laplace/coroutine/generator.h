/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_COROUTINE_GENERATOR_H
#define LAPLACE_COROUTINE_GENERATOR_H

#include <coroutine>
#include <exception>
#include <iterator>

namespace laplace::coroutine {
  template <typename return_type_>
  struct generator {
    struct promise_type {
      using handle_type = std::coroutine_handle<promise_type>;

      int                ref_count = 0;
      return_type_       value     = {};
      std::exception_ptr exception = {};

      auto initial_suspend() noexcept;
      auto final_suspend() noexcept;
      auto yield_value(return_type_ const &_value) noexcept;
      auto yield_value(return_type_ &&_value) noexcept;
      auto return_value(return_type_ const &_value) noexcept;
      auto return_value(return_type_ &&_value) noexcept;
      auto get_return_object() noexcept;
      auto unhandled_exception() noexcept;

      static void free_handle(handle_type &handle) noexcept;
    };

    using handle_type = typename promise_type::handle_type;

    struct iterator {
      iterator(iterator const &) = delete;
      iterator &operator=(iterator const &) = delete;

      iterator(iterator &&other) noexcept;
      auto operator=(iterator &&other) noexcept -> iterator &;

      explicit iterator(handle_type handle) noexcept;
      ~iterator() noexcept;

      auto               operator++() noexcept;
      [[nodiscard]] auto operator*();
      [[nodiscard]] auto operator!=(std::default_sentinel_t) noexcept;

    private:
      handle_type m_handle;
    };

    generator(generator const &) noexcept;
    generator(generator &&) noexcept;
    auto operator=(generator const &) noexcept -> generator &;
    auto operator=(generator &&) noexcept -> generator &;

    explicit generator(handle_type handle) noexcept;
    ~generator() noexcept;

    [[nodiscard]] auto await_ready() const noexcept;
    auto await_suspend(std::coroutine_handle<> handle) noexcept;
    auto await_resume();

    [[nodiscard]] auto begin() noexcept;
    [[nodiscard]] auto end() noexcept;

    [[nodiscard]] auto get();
    [[nodiscard]] auto is_done() const noexcept;

  private:
    void resume() noexcept;

    handle_type m_handle;
  };
}

#include "generator.impl.h"

#endif
