/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "loader.h"

namespace laplace::engine {
  using std::shared_lock, std::unique_lock, std::jthread;

  loader::loader() noexcept {
    auto loop = [this]() {
      auto _ul = unique_lock(m_lock);

      while (!m_is_done) {
        while (!m_queue.empty()) {
          if (!m_context.decode || !m_context.perform) {
            log(log_event::error, "Invalid context.", __FUNCTION__);
            break;
          }

          auto task = m_context.decode(m_queue.front());
          m_queue.erase(m_queue.begin());

          if (task) {
            _ul.unlock();
            m_context.perform(task);
            _ul.lock();
          }

          m_progress++;
        }

        m_is_ready = true;

        if (m_is_done)
          /*  Break to prevent freeze.
           */
          break;

        m_sync.wait(_ul);
      }
    };

    for (int i = 0; i < 4; i++) {
      try {
        m_thread = jthread(loop);
        break;
      } catch (std::system_error &error) {
        log(log_event::error, "Unable to create a thread.",
            __FUNCTION__);
        log(log_event::error, error.what(), __FUNCTION__);
        std::this_thread::yield();
      }
    }
  }

  loader::~loader() noexcept {
    done();
  }

  void loader::on_decode(fn_decode const &decode) noexcept {
    auto _ul         = unique_lock(m_lock);
    m_context.decode = decode;
  }

  void loader::on_perform(fn_perform const &perform) noexcept {
    auto _ul          = unique_lock(m_lock);
    m_context.perform = perform;
  }

  void loader::add_task(span_cbyte task) noexcept {
    auto _ul   = unique_lock(m_lock);
    m_is_ready = false;
    m_queue.emplace_back(vbyte { task.begin(), task.end() });
    m_sync.notify_all();
  }

  auto loader::is_ready() noexcept -> bool {
    auto _ul = unique_lock(m_lock);
    return m_is_ready;
  }

  auto loader::get_progress() noexcept -> sl::index {
    auto _ul = unique_lock(m_lock);
    return m_progress;
  }

  void loader::done() noexcept {
    auto _ul  = unique_lock(m_lock);
    m_is_done = true;
    m_sync.notify_all();
  }
}
