#include "execution.h"

#include "impact.h"
#include <utility>

namespace laplace {
  using std::pmr::memory_resource, std::thread, std::max,
      std::function, std::visit, std::decay_t, std::is_same_v,
      std::remove_if, std::pmr::vector, std::lower_bound, std::sort,
      std::unique_lock, std::jthread, std::optional, std::nullopt;

  execution::execution() noexcept {
    _init();
  }

  execution::execution(memory_resource *resource) noexcept :
      m_sync(average_sync_impact_count(), resource),
      m_async(average_async_impact_count(), resource),
      m_forks(resource), m_threads(resource), m_queue(resource) {
    _init();
  }

  execution::execution(execution const &exe) noexcept {
    _assign(exe);
  }

  execution::execution(execution &&exe) noexcept {
    _assign(std::move(exe));
  }

  execution::~execution() noexcept {
    _done_and_notify();
  }

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

  auto execution::error() const noexcept -> bool {
    return m_is_error;
  }

  auto execution::thread_count() const noexcept -> ptrdiff_t {
    return _bind<ptrdiff_t>(
        [&]() -> ptrdiff_t { return m_threads.size(); }, 0);
  }

  auto execution::set_thread_count(
      ptrdiff_t thread_count) const noexcept -> execution {
    return _bind([&]() {
      if (thread_count < 0)
        return _error();
      if (thread_count >
          overthreading_limit() *
              max<ptrdiff_t>(1, thread::hardware_concurrency()))
        return _error();

      auto exe = execution { *this };
      exe._init_threads(thread_count);
      return exe;
    });
  }

  auto execution::set_state(state s) const noexcept -> execution {
    return _bind([&]() {
      auto exe    = execution { *this };
      exe.m_state = std::move(s);
      return exe;
    });
  }

  auto execution::read_only() const noexcept -> access {
    return _bind<access>([&]() { return access { m_state }; },
                         access {});
  }

  void execution::queue(action const &a) noexcept {
    m_queue.emplace_back(action_state {
        .index         = static_cast<ptrdiff_t>(m_queue.size()),
        .tick_duration = a.tick_duration(),
        .generator     = a.run(a.self()) });
  }

  void execution::schedule(time_type time) noexcept {
    if (m_threads.empty()) {
      auto sync  = impact_list { average_sync_impact_count(),
                                default_memory_resource() };
      auto async = impact_list { average_async_impact_count(),
                                 default_memory_resource() };
      auto forks = vector<action_state> { default_memory_resource() };
      forks.reserve(average_fork_count());

      for (; time > 0; time--) {
        for (bool done = false; !done;) {
          done = true;

          sync.clear();
          async.clear();
          forks.clear();

          for (auto &a : m_queue) {
            if (a.generator.is_done() || a.clock != 0)
              continue;

            bool is_continue = false;

            for (auto const &i : a.generator.next())
              visit(
                  [&](auto const &v) {
                    using type = decay_t<decltype(v)>;

                    if constexpr (is_same_v<type, tick_continue>) {
                      is_continue = true;
                      done        = false;
                    } else if constexpr (is_same_v<type,
                                                   queue_action>) {
                      forks.emplace(
                          lower_bound(forks.begin(), forks.end(),
                                      a.index,
                                      [](auto const &f, auto n) {
                                        return f.index < n;
                                      }),
                          action_state { .tick_duration =
                                             v.value.tick_duration(),
                                         .generator = v.value.run(
                                             v.value.self()) });
                      done = false;
                    } else if constexpr (mode_of<type>() ==
                                         mode::sync)
                      sync += i;
                    else
                      async += i;
                  },
                  i.value);

            if (!is_continue)
              a.clock = a.tick_duration;
          }

          for (auto &i : sync)
            if (!m_state.apply(i))
              _set_error();

          for (auto &i : async)
            if (!m_state.apply(i))
              _set_error();

          for (auto n = ptrdiff_t {}; n < forks.size(); n++)
            forks[n].index = m_queue.size() + n;

          m_queue.insert(m_queue.end(), forks.begin(), forks.end());

          if (sync.size() > 0 || async.size() > 0) {
            while (m_state.adjust()) { }
            m_state.adjust_done();
          }
        }

        m_queue.erase(
            remove_if(m_queue.begin(), m_queue.end(),
                      [](auto &a) { return a.generator.is_done(); }),
            m_queue.end());

        for (auto n = ptrdiff_t {}; n < m_queue.size(); n++) {
          m_queue[n].index = n;
          m_queue[n].clock--;
        }
      }
    } else {
      auto _ul = unique_lock(m_lock);
      m_ticks += time;
      _ul.unlock();
      m_on_tick.notify_all();
    }
  }

  void execution::join() noexcept {
    if (m_threads.empty())
      return;
    auto _ul = unique_lock(m_lock);
    m_on_join.wait(_ul, [&]() { return m_is_done || m_ticks == 0; });
  }

  void execution::schedule_and_join(time_type time) noexcept {
    schedule(time);
    join();
  }

  auto execution::_error() const noexcept -> execution {
    auto exe = execution { *this };
    exe._set_error();
    return exe;
  }

  auto execution::_bind(function<execution()> f) const noexcept
      -> execution {
    if (error())
      return *this;
    return f();
  }

  template <typename type_>
  auto execution::_bind(function<type_()> f, type_ def) const noexcept
      -> type_ {
    if (error())
      return def;
    return f();
  }

  void execution::_once(std::function<void()> f) noexcept {
    auto _ul = unique_lock(m_lock);
    if (++m_fence_in == m_threads.size()) {
      _ul.unlock();
      f();
      m_on_fence.notify_all();
      _ul.lock();
    } else
      m_on_fence.wait(_ul);
    if (++m_fence_out == m_threads.size()) {
      m_fence_in  = 0;
      m_fence_out = 0;
      _ul.unlock();
      m_on_fence.notify_all();
    } else
      m_on_fence.wait(_ul);
  }

  void execution::_done_and_notify() noexcept {
    if (m_is_done)
      return;
    auto _ul  = unique_lock(m_lock);
    m_is_done = true;
    _ul.unlock();
    m_on_tick.notify_all();
    m_on_join.notify_all();
  }

  void execution::_stop_threads() noexcept {
    _done_and_notify();
    for (auto &t : m_threads) t.join();
  }

  auto execution::_next_action() noexcept -> optional<ptrdiff_t> {
    auto _ul = unique_lock(m_lock);
    if (m_queue_index == m_queue.size())
      return nullopt;
    return m_queue_index++;
  }

  void execution::_add_sync(impact const &i) noexcept {
    auto _ul = unique_lock(m_lock);
    m_sync += i;
  }

  void execution::_add_async(impact const &i) noexcept {
    auto _ul = unique_lock(m_lock);
    m_async += i;
  }

  auto execution::_add_impacts(impact_list const &list) noexcept
      -> tick_status {
    auto s = tick_status::done;
    for (auto &i : list)
      visit(
          [&](auto const &v) {
            using type = decay_t<decltype(v)>;

            if constexpr (is_same_v<type, tick_continue>)
              s = tick_status::continue_;
            else if constexpr (mode_of<type>() == mode::sync)
              _add_sync(i);
            else
              _add_async(i);
          },
          i.value);
    return s;
  }

  auto execution::_perform(ptrdiff_t index) noexcept -> bool {
    auto &a         = m_queue[index];
    bool  performed = a.clock == 0;
    if (performed &&
        _add_impacts(a.generator.next()) == tick_status::done)
      a.clock = a.tick_duration;
    return performed;
  }

  auto execution::_next_async() noexcept -> optional<ptrdiff_t> {
    auto _ul = unique_lock(m_lock);
    if (m_async_index == m_async.size())
      return nullopt;
    return m_async_index++;
  }

  auto execution::_dec_tick() noexcept -> bool {
    auto _ul = unique_lock(m_lock);
    return --m_ticks == 0;
  }

  auto execution::_tick_done() noexcept -> bool {
    auto _ul = unique_lock(m_lock);
    return m_is_tick_done;
  }

  void execution::_add_tick_done(bool done) noexcept {
    auto _ul = unique_lock(m_lock);
    m_is_tick_done &= done;
  }

  auto execution::_perform_actions() noexcept -> bool {
    _once([&]() { m_is_tick_done = true; });

    bool tick_not_done = false;

    while (auto n = _next_action()) tick_not_done |= _perform(*n);

    _add_tick_done(!tick_not_done);

    _once([&]() { m_queue_index = 0; });

    return !_tick_done();
  }

  void execution::_apply_impacts() noexcept {
    _once([&]() {
      for (auto &i : m_sync) std::ignore = m_state.apply(i);
      m_sync.clear();
    });

    while (auto n = _next_async())
      std::ignore = m_state.apply(m_async[*n]);

    _once([&]() {
      m_async_index = 0;
      m_async.clear();
    });

    while (m_state.adjust()) { }

    _once([&]() { m_state.adjust_done(); });
  }

  void execution::_apply_time() noexcept {
    while (auto n = _next_action()) m_queue[*n].clock--;

    _once([&]() {
      m_queue_index = 0;

      if (_dec_tick())
        m_on_join.notify_all();
    });
  }

  void execution::_thread() noexcept {
    auto _ul = unique_lock(m_lock);
    while (!m_is_done) {
      m_on_tick.wait(_ul,
                     [&]() { return m_is_done || m_ticks != 0; });
      while (m_ticks != 0) {
        _ul.unlock();

        while (_perform_actions()) _apply_impacts();

        _apply_time();

        _ul.lock();
      }
    }
  }

  void execution::_init_threads(ptrdiff_t thread_count) noexcept {
    if (m_is_error)
      return;
    _stop_threads();
    m_is_done = false;
    m_threads.resize(thread_count);
    for (auto &t : m_threads)
      t = jthread { [this]() { this->_thread(); } };
  }

  void execution::_init() noexcept {
    m_forks.reserve(average_fork_count());
    _init_threads(default_thread_count());
  }

  void execution::_assign(execution const &exe) noexcept {
    m_is_error = exe.m_is_error;
    m_state    = exe.m_state;
    m_queue    = exe.m_queue;

    _init_threads(exe.m_threads.size());
  }

  void execution::_assign(execution &&exe) noexcept {
    exe.join();

    m_is_error = exe.m_is_error;
    m_state    = std::move(exe.m_state);
    m_queue    = std::move(exe.m_queue);

    _init_threads(exe.m_threads.size());

    exe._set_error();
  }

  void execution::_set_error() noexcept {
    m_is_error = true;
    _done_and_notify();
  }
}
