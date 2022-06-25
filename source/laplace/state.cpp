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

  auto state::is_error() const noexcept -> bool {
    return false;
  }

  bool state::apply(impact const &i) noexcept {
    return std::visit(
        overload { [&](noop const &i) { return true; },
                   [&](integer_reallocate const &i) {
                     return m_integers.reallocate(i.id, i.size);
                   },
                   [&](integer_allocate const &i) {
                     return m_integers.set(
                         i.return_id, i.return_index,
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
                   } },
        i);
  }

  auto state::get_integer(ptrdiff_t id, ptrdiff_t index,
                          value_type def) const noexcept
      -> value_type {
    return m_integers.get(id, index, def);
  }

  auto state::adjust() noexcept -> bool {
    return m_integers.adjust();
  }

  void state::adjust_done() noexcept {
    m_integers.adjust_done();
  }
}
