#ifndef LAPLACE_X_ACCESS_IMPL_H
#define LAPLACE_X_ACCESS_IMPL_H

namespace laplace {
  inline access::access(state const &s) noexcept {
    if (auto p = s.io(); p)
      m_io = p;
  }

  inline auto access::get_integer(ptrdiff_t id, ptrdiff_t index,
                                  int_type def) const noexcept
      -> int_type {
    if (!m_io)
      return def;
    return (*m_io)->get_integer(id, index, def);
  }

  inline auto access::get_byte(ptrdiff_t id, ptrdiff_t index,
                               byte_type def) const noexcept
      -> byte_type {
    if (!m_io)
      return def;
    return (*m_io)->get_byte(id, index, def);
  }
}

#endif
