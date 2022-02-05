/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_engine_loader_h
#define laplace_engine_loader_h

#include "prime_impact.h"
#include <condition_variable>
#include <mutex>
#include <thread>

namespace laplace::engine {
  /*  Queued background loading.
   */
  class loader {
  public:
    using fn_decode  = std::function<ptr_prime_impact(span_cbyte)>;
    using fn_perform = std::function<void(ptr_prime_impact const &)>;

    struct context {
      fn_decode  decode;
      fn_perform perform;
    };

    loader() noexcept;
    ~loader() noexcept;

    void on_decode(fn_decode const &decode) noexcept;
    void on_perform(fn_perform const &perform) noexcept;

    void add_task(span_cbyte task) noexcept;

    /*  Check if all tasks done.
     */
    [[nodiscard]] auto is_ready() noexcept -> bool;

    /*  Get done task count.
     */
    [[nodiscard]] auto get_progress() noexcept -> sl::index;

  private:
    void done() noexcept;

    std::mutex              m_lock;
    std::condition_variable m_sync;
    std::jthread            m_thread;

    bool              m_is_ready = true;
    bool              m_is_done  = false;
    sl::index         m_progress = 0;
    context           m_context;
    sl::vector<vbyte> m_queue;
  };
}

#endif
