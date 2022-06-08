/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_EXECUTION_H
#define LAPLACE_EXECUTION_H

#include <cstddef>
#include <shared_mutex>
#include <thread>
#include <vector>

namespace laplace {
  class execution {
  public:
    static const ptrdiff_t default_thread_count;
    static const ptrdiff_t overthreading_limit;

    execution() noexcept;
    execution(execution const &exe) noexcept;
    execution(execution &&exe) noexcept;
    ~execution() noexcept;

    auto operator=(execution const &exe) noexcept -> execution &;
    auto operator=(execution &&exe) noexcept -> execution &;

    [[nodiscard]] auto is_error() const noexcept -> bool;

    [[nodiscard]] auto get_thread_count() const noexcept -> ptrdiff_t;

    [[nodiscard]] auto set_thread_count(
        ptrdiff_t thread_count) const noexcept -> execution;

  private:
    [[nodiscard]] static auto _error() noexcept -> execution;
    [[nodiscard]] auto        _is_done() noexcept -> bool;

    void _init_threads(signed long long thread_count) noexcept;
    void _assign(execution const &exe) noexcept;
    void _assign(execution &&exe) noexcept;
    void _cleanup() noexcept;
    void _set_done(bool is_done) noexcept;
    void _set_error() noexcept;

    bool                      m_is_error = false;
    bool                      m_is_done  = false;
    std::vector<std::jthread> m_threads;
    std::shared_mutex         m_mutex;
  };
}

#endif
