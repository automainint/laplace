/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "execution.h"

#include <utility>

namespace laplace {
  using std::thread, std::max;

  const ptrdiff_t execution::default_thread_count = 3;
  const ptrdiff_t execution::overthreading_limit  = 8;

  execution::execution() noexcept { }

  execution::execution(execution const &exe) noexcept {
    _assign(exe);
  }

  execution::execution(execution &&exe) noexcept {
    _assign(std::move(exe));
  }

  execution::~execution() noexcept { }

  auto execution::operator=(execution const &exe) noexcept
      -> execution & {
    if (this == &exe)
      return *this;

    _assign(exe);
    return *this;
  }

  auto execution::operator=(execution &&exe) noexcept -> execution & {
    _assign(std::move(exe));
    return *this;
  }

  auto execution::is_error() const noexcept -> bool {
    return m_is_error;
  }

  auto execution::get_thread_count() const noexcept -> ptrdiff_t {
    return m_thread_count;
  }

  auto execution::set_thread_count(
      ptrdiff_t thread_count) const noexcept -> execution {
    if (is_error())
      return *this;
    if (thread_count < 0)
      return _error();
    if (thread_count >
        overthreading_limit *
            max<ptrdiff_t>(1, thread::hardware_concurrency()))
      return _error();

    auto exe           = execution {};
    exe.m_thread_count = thread_count;
    return exe;
  }

  auto execution::_error() noexcept -> execution {
    auto exe = execution {};
    exe._set_error();
    return exe;
  }

  void execution::_assign(execution const &exe) noexcept {
    m_is_error     = exe.m_is_error;
    m_is_done      = exe.m_is_done;
    m_thread_count = exe.m_thread_count;
    m_action       = exe.m_action;
  }

  void execution::_assign(execution &&exe) noexcept {
    m_is_error     = exe.m_is_error;
    m_is_done      = exe.m_is_done;
    m_thread_count = exe.m_thread_count;
    m_action       = std::move(exe.m_action);

    exe._set_error();
  }

  void execution::_set_error() noexcept {
    m_is_error = true;
  }

  void execution::queue(action a) noexcept {
    m_action = std::move(a);
  }

  void execution::schedule(time_type time) noexcept {
    std::ignore = m_action.run(m_action.get_self()).begin();
  }

  void execution::join() noexcept { }
}
