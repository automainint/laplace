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

#include "iterator.h"
#include "utils.h"

namespace laplace::coroutine {
  template <typename type_>
  struct generator {
    struct promise_type {
      using handle_type = std::coroutine_handle<promise_type>;

      type_              m_value     = {};
      int                m_ref_count = 0;
      std::exception_ptr m_exception = nullptr;
      handle_type        m_nested    = nullptr;

      auto initial_suspend() noexcept;
      auto final_suspend() noexcept;
      auto get_return_object() noexcept;
      auto unhandled_exception() noexcept;
      auto return_value(type_ const &value) noexcept;
      auto return_value(type_ &&value) noexcept;
      auto yield_value(type_ const &value) noexcept;
      auto yield_value(type_ &&value) noexcept;
      auto yield_value(generator<type_> gen) noexcept;
    };

    using handle_type = typename promise_type::handle_type;

    generator(generator const &other) noexcept;
    generator(generator &&other) noexcept;
    auto operator=(generator const &other) noexcept -> generator &;
    auto operator=(generator &&other) noexcept -> generator &;

    explicit generator(handle_type handle) noexcept;
    ~generator() noexcept;

    [[nodiscard]] auto await_ready() noexcept;
    auto await_suspend(std::coroutine_handle<>) noexcept;
    auto await_resume();

    [[nodiscard]] auto begin() noexcept;
    [[nodiscard]] auto end() noexcept;

    [[nodiscard]] auto next();
    [[nodiscard]] auto get() noexcept;
    [[nodiscard]] auto is_done() noexcept;

  private:
    handle_type m_handle;
  };
}

#include "generator.impl.h"

#endif
