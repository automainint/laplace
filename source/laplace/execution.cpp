#include "execution.h"

#include "impact.h"
#include <utility>

namespace laplace {
  using std::thread, std::max, std::function, std::visit,
      std::decay_t, std::is_same_v;

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
    m_action = action_state { .tick_duration = a.tick_duration(),
                              .generator     = a.run(a.self()) };
  }

  void execution::schedule(time_type time) noexcept {
    if (!m_action)
      return;

    for (; time > 0; time--) {
      if (m_action->clock == 0) {
        for (auto &i : m_action->generator.next())
          visit(
              [&](auto const &a) {
                using type = decay_t<decltype(i)>;

                if constexpr (is_same_v<type, tick_continue>)
                  return;
                if constexpr (is_same_v<type, queue_action>)
                  return;

                std::ignore = m_state.apply(impact { a });
              },
              i.value);
        m_action->clock = m_action->tick_duration;
      } else
        m_action->clock--;

      while (m_state.adjust()) { }
      m_state.adjust_done();
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
    if (is_error())
      return *this;
    return f();
  }

  template <typename type_>
  auto execution::_bind(function<type_()> f, type_ def) const noexcept
      -> type_ {
    if (is_error())
      return def;
    return f();
  }

  void execution::_assign(execution const &exe) noexcept {
    m_is_error     = exe.m_is_error;
    m_is_done      = exe.m_is_done;
    m_thread_count = exe.m_thread_count;
    m_action       = exe.m_action;
    m_state        = exe.m_state;
  }

  void execution::_assign(execution &&exe) noexcept {
    m_is_error     = exe.m_is_error;
    m_is_done      = exe.m_is_done;
    m_thread_count = exe.m_thread_count;
    m_action       = std::move(exe.m_action);
    m_state        = std::move(exe.m_state);

    exe._set_error();
  }

  void execution::_set_error() noexcept {
    m_is_error = true;
  }
}
