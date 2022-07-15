#include "execution.h"

#include "impact.h"
#include <utility>

namespace laplace {
  using std::pmr::memory_resource, std::thread, std::max,
      std::function, std::visit, std::decay_t, std::is_same_v,
      std::remove_if, std::pmr::vector, std::lower_bound, std::sort;

  execution::execution() noexcept { }

  execution::execution(memory_resource *resource) noexcept :
      m_queue(resource) { }

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

  auto execution::error() const noexcept -> bool {
    return m_is_error;
  }

  auto execution::thread_count() const noexcept -> ptrdiff_t {
    return m_thread_count;
  }

  auto execution::set_thread_count(
      ptrdiff_t thread_count) const noexcept -> execution {
    return _bind([&]() {
      if (thread_count < 0)
        return _error();
      if (thread_count >
          overthreading_limit *
              max<ptrdiff_t>(1, thread::hardware_concurrency()))
        return _error();

      auto exe           = execution { *this };
      exe.m_thread_count = thread_count;
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
    auto sync  = impact_list { average_sync_impact_count,
                              default_memory_resource() };
    auto async = impact_list { average_async_impact_count,
                               default_memory_resource() };
    auto forks = vector<action_state> { default_memory_resource() };
    forks.reserve(average_fork_count);

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
                        action_state {
                            .tick_duration = v.value.tick_duration(),
                            .generator     = v.value.run(
                                    v.value.self()) });
                    done = false;
                  } else if constexpr (mode_of<type>() == mode::sync)
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
  }

  void execution::join() noexcept { }

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

  void execution::_assign(execution const &exe) noexcept {
    m_is_error     = exe.m_is_error;
    m_is_done      = exe.m_is_done;
    m_thread_count = exe.m_thread_count;
    m_queue        = exe.m_queue;
    m_state        = exe.m_state;
  }

  void execution::_assign(execution &&exe) noexcept {
    m_is_error     = exe.m_is_error;
    m_is_done      = exe.m_is_done;
    m_thread_count = exe.m_thread_count;
    m_queue        = std::move(exe.m_queue);
    m_state        = std::move(exe.m_state);

    exe._set_error();
  }

  void execution::_set_error() noexcept {
    m_is_error = true;
  }
}
