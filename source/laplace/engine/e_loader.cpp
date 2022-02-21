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
  using std::unique_lock, std::jthread, std::move;

  loader::loader(loader &&other) noexcept {
    auto _ul   = unique_lock(other.m_lock);
    m_is_ready = other.m_is_ready;
    m_is_done  = other.m_is_done;
    m_progress = other.m_progress;
    m_context  = move(other.m_context);
    m_queue    = move(other.m_queue);

    other.m_is_done = true;
    other.m_sync.notify_all();
    _ul.unlock();

    _init();
  }

  auto loader::operator=(loader &&other) noexcept -> loader & {
    auto _ul       = unique_lock(m_lock, std::defer_lock);
    auto _ul_other = unique_lock(other.m_lock, std::defer_lock);
    std::lock(_ul, _ul_other);

    m_is_ready = other.m_is_ready;
    m_is_done  = other.m_is_done;
    m_progress = other.m_progress;
    m_context  = move(other.m_context);
    m_queue    = move(other.m_queue);

    other.m_is_done = true;
    other.m_sync.notify_all();
    _ul_other.unlock();

    _init();

    return *this;
  }

  loader::loader() noexcept {
    _init();
  }

  loader::~loader() noexcept {
    _done();
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
    auto _ul = unique_lock(m_lock);

    if (!m_thread.joinable()) {
      log(log_event::warning,
          "No thread! Performing the task sequentially.",
          __FUNCTION__);
      _perform(task, _ul);
      return;
    }

    _queue(task);
  }

  auto loader::is_ready() noexcept -> bool {
    auto _ul = unique_lock(m_lock);
    return m_is_ready;
  }

  auto loader::get_progress() noexcept -> sl::index {
    auto _ul = unique_lock(m_lock);
    return m_progress;
  }

  void loader::_init() noexcept {
    for (int i = 0; i < 4; i++) {
      try {
        m_thread = jthread([this]() { _loop(); });
        break;
      } catch (std::system_error &error) {
        log(log_event::fatal, "Unable to create a thread.",
            __FUNCTION__);
        log(log_event::fatal, error.what(), __FUNCTION__);
        std::this_thread::yield();
      }
    }
  }

  void loader::_queue(span_cbyte task) noexcept {
    m_is_ready = false;
    m_queue.emplace_back(vbyte { task.begin(), task.end() });
    m_sync.notify_all();
  }

  void loader::_perform(span_cbyte task, auto &_ul) noexcept {
    if (!m_context.decode || !m_context.perform) {
      log(log_event::error, "Invalid context.", __FUNCTION__);
      return;
    }

    auto decoded = m_context.decode(task);
    auto perform = m_context.perform;

    _ul.unlock();
    perform(decoded);
    _ul.lock();

    m_progress++;
  }

  void loader::_loop() noexcept {
    auto _ul = unique_lock(m_lock);

    while (!m_is_done) {
      while (!m_queue.empty()) {
        if (!m_context.decode || !m_context.perform) {
          log(log_event::error, "Invalid context.", __FUNCTION__);
          break;
        }

        auto task = std::move(m_queue.front());
        m_queue.erase(m_queue.begin());

        _perform(task, _ul);
      }

      m_is_ready = true;

      if (m_is_done)
        /*  Break to prevent freeze.
         */
        break;

      m_sync.wait(_ul);
    }
  }

  void loader::_done() noexcept {
    auto _ul  = unique_lock(m_lock);
    m_is_done = true;
    m_sync.notify_all();
  }
}
