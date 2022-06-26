/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "state.h"

namespace laplace {
  template <class... types_>
  struct overload : types_... {
    using types_::operator()...;
  };

  template <class... types_>
  overload(types_...) -> overload<types_...>;

  state::state() noexcept {
    m_random.seed(std::random_device {}());
  }

  auto state::set_seed(int_type seed) const noexcept -> state {
    auto s = state { *this };
    s.m_random.seed(seed);
    return s;
  }

  auto state::get_integer(ptrdiff_t id, ptrdiff_t index,
                          int_type def) const noexcept -> int_type {
    return m_integers.get(id, index, def);
  }

  auto state::get_byte(ptrdiff_t id, ptrdiff_t index,
                       byte_type def) const noexcept -> byte_type {
    return m_bytes.get(id, index, def);
  }

  auto state::apply(impact const &i) noexcept -> bool {
    return std::visit(
        overload {
            [&](noop const &i) { return true; },
            [&](tick_continue const &i) { return false; },
            [&](integer_reserve const &i) {
              return m_integers.reserve(i.count);
            },
            [&](integer_reallocate const &i) {
              return m_integers.reallocate(i.id, i.size);
            },
            [&](integer_allocate const &i) {
              return m_integers.set(i.return_id, i.return_index,
                                    m_integers.allocate(i.size));
            },
            [&](integer_deallocate const &i) {
              return m_integers.deallocate(i.id);
            },
            [&](integer_set const &i) {
              return m_integers.set(i.id, i.index, i.value);
            },
            [&](integer_add const &i) {
              return m_integers.add(i.id, i.index, i.delta);
            },
            [&](byte_reserve const &i) {
              return m_bytes.reserve(i.count);
            },
            [&](byte_reallocate const &i) {
              return m_bytes.reallocate(i.id, i.size);
            },
            [&](byte_allocate const &i) {
              return m_integers.set(i.return_id, i.return_index,
                                    m_bytes.allocate(i.size));
            },
            [&](byte_deallocate const &i) {
              return m_bytes.deallocate(i.id);
            },
            [&](byte_set const &i) {
              return m_bytes.set(i.id, i.index, i.value);
            },
            [&](byte_add const &i) {
              return m_bytes.add(i.id, i.index, i.delta);
            },
            [&](random const &i) {
              auto distribute =
                  std::uniform_int_distribution<int_type> { i.min,
                                                            i.max };
              for (ptrdiff_t k = 0; k < i.return_size; k++)
                if (!m_integers.set(i.return_id, i.return_index + k,
                                    distribute(m_random)))
                  return false;
              return true;
            } },
        i);
  }

  auto state::adjust() noexcept -> bool {
    return m_integers.adjust() || m_bytes.adjust();
  }

  void state::adjust_done() noexcept {
    m_integers.adjust_done();
    m_bytes.adjust_done();
  }
}
