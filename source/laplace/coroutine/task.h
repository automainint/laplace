/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_COROUTINE_TASK_H
#define LAPLACE_COROUTINE_TASK_H

#include <coroutine>
#include <exception>

namespace laplace::coroutine {
  template <typename return_type_ = void>
  struct task {
    struct promise_type {
      int                ref_count = 0;
      return_type_       value     = {};
      std::exception_ptr exception = {};

      auto initial_suspend() noexcept;
      auto final_suspend() noexcept;
      auto return_value(return_type_ const &_value) noexcept;
      auto return_value(return_type_ &&_value) noexcept;
      auto get_return_object() noexcept;
      auto unhandled_exception() noexcept;
    };

    using handle_type = std::coroutine_handle<promise_type>;

    task(task const &) noexcept;
    task(task &&) noexcept;
    auto operator=(task const &) noexcept -> task &;
    auto operator=(task &&) noexcept -> task &;

    explicit task(handle_type _handle) noexcept;
    ~task() noexcept;

    auto await_ready() const noexcept;
    auto await_suspend(std::coroutine_handle<> handle) noexcept;
    auto await_resume() noexcept;

    auto is_done() noexcept;
    void resume() noexcept;
    auto get() noexcept;

  private:
    void free_handle() noexcept;

    handle_type m_handle;
  };

  template <>
  struct task<void> {
    struct promise_type {
      int                ref_count = 0;
      std::exception_ptr exception = {};

      auto initial_suspend() noexcept;
      auto final_suspend() noexcept;
      auto return_void() noexcept;
      auto get_return_object() noexcept;
      auto unhandled_exception() noexcept;
    };

    using handle_type = std::coroutine_handle<promise_type>;

    task(task const &) noexcept;
    task(task &&) noexcept;
    auto operator=(task const &) noexcept -> task &;
    auto operator=(task &&) noexcept -> task &;

    explicit task(handle_type _handle) noexcept;
    ~task() noexcept;

    auto await_ready() const noexcept;
    auto await_suspend(std::coroutine_handle<> handle) noexcept;
    auto await_resume() noexcept;

    auto is_done() noexcept;
    void resume() noexcept;

  private:
    void free_handle() noexcept;

    handle_type m_handle;
  };
}

#include "task.impl.h"

#endif
