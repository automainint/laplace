/*  laplace/engine/e_loader.cpp
 *
 *  Copyright (c) 2021 Mitya Selivanov
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
  using std::shared_lock, std::unique_lock, std::thread;

  loader::loader() {
    m_thread = thread([this]() {
      auto _ul = unique_lock(m_lock);

      while (!m_is_done) {
        while (!m_queue.empty()) {
          auto task = ptr_impact { m_queue.front() };
          m_queue.erase(m_queue.begin());

          if (m_world) {
            _ul.unlock();

            task->perform({ *m_world, access::data });

            _ul.lock();
            m_progress++;
          } else {
            error(__FUNCTION__, "No world.");
          }
        }

        m_is_ready = true;
        m_sync.wait(_ul);
      }
    });
  }

  loader::~loader() {
    done();

    if (m_thread.joinable()) {
      m_thread.join();
    }
  }

  void loader::set_world(ptr_world w) {
    auto _ul = unique_lock(m_lock);
    m_world  = w;
  }

  void loader::add_task(ptr_impact task) {
    auto _ul   = unique_lock(m_lock);
    m_is_ready = false;
    m_queue.emplace_back(task);
    m_sync.notify_all();
  }

  auto loader::is_ready() -> bool {
    auto _ul = unique_lock(m_lock);
    return m_is_ready;
  }

  auto loader::get_progress() -> size_t {
    auto _ul = unique_lock(m_lock);
    return m_progress;
  }

  void loader::set_ready(bool is_ready) {
    auto _ul   = unique_lock(m_lock);
    m_is_ready = is_ready;
    m_sync.notify_all();
  }

  void loader::done() {
    auto _ul  = unique_lock(m_lock);
    m_is_done = true;
    m_sync.notify_all();
  }
}
