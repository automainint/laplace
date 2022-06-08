/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "execution.h"

#include <mutex>
#include <stdexcept>

namespace laplace {
  using std::jthread, std::this_thread::sleep_for,
      std::chrono::milliseconds, std::bad_alloc, std::system_error,
      std::unique_lock, std::shared_lock, std::thread, std::min,
      std::max;

  const ptrdiff_t execution::default_thread_count = 3;
  const ptrdiff_t execution::overthreading_limit  = 8;

  execution::execution() noexcept {
    _init_threads(default_thread_count);
  }

  execution::execution(execution const &exe) noexcept {
    _assign(exe);
  }

  execution::execution(execution &&exe) noexcept {
    _assign(exe);
  }

  execution::~execution() noexcept {
    _set_done(true);
  }

  auto execution::operator=(execution const &exe) noexcept
      -> execution & {
    if (this == &exe)
      return *this;

    _cleanup();
    _assign(exe);
    return *this;
  }

  auto execution::operator=(execution &&exe) noexcept -> execution & {
    _cleanup();
    _assign(exe);
    return *this;
  }

  auto execution::is_error() const noexcept -> bool {
    return m_is_error;
  }

  auto execution::get_thread_count() const noexcept -> ptrdiff_t {
    return static_cast<signed long long>(m_threads.size());
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

    auto exe = execution {};
    exe._init_threads(thread_count);
    return exe;
  }

  auto execution::_error() noexcept -> execution {
    auto exe = execution {};
    exe._set_error();
    return exe;
  }

  auto execution::_is_done() noexcept -> bool {
    auto _sl = shared_lock { m_mutex };
    return m_is_done;
  }

  void execution::_init_threads(
      signed long long thread_count) noexcept {
    try {
      _set_done(true);

      for (auto &thread : m_threads)
        if (thread.joinable())
          thread.join();

      _set_done(false);

      m_threads.resize(min<ptrdiff_t>(thread_count,
                                      thread::hardware_concurrency() *
                                          overthreading_limit));

      for (auto &thread : m_threads)
        thread = jthread { [&]() {
          while (!_is_done()) sleep_for(milliseconds(1));
        } };

    } catch (bad_alloc &) { _set_error(); } catch (system_error &) {
      _set_error();
    }
  }

  void execution::_assign(execution const &exe) noexcept {
    if (exe.is_error()) {
      _set_error();
      return;
    }

    _init_threads(exe.get_thread_count());
  }

  void execution::_assign(execution &&exe) noexcept {
    if (exe.is_error()) {
      _set_error();
      return;
    }

    _init_threads(exe.get_thread_count());

    exe._set_error();
  }

  void execution::_cleanup() noexcept {
    _set_done(true);
  }

  void execution::_set_done(bool is_done) noexcept {
    auto _ul  = unique_lock { m_mutex };
    m_is_done = is_done;
  }

  void execution::_set_error() noexcept {
    _set_done(true);
    m_is_error = true;
  }
}
