/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_IMPACT_IMPL_H
#define LAPLACE_IMPACT_IMPL_H

namespace laplace {
  inline impact_list::impact_list(impact i) noexcept :
      m_data(&m_resource) {
    m_data.reserve(average_impact_list_size);
    m_data.emplace_back(i);
  }

  inline auto impact_list::size() const noexcept -> ptrdiff_t {
    return m_data.size();
  }

  inline auto impact_list::operator[](ptrdiff_t index) const noexcept
      -> impact const & {
    return m_data[index];
  }

  inline auto impact_list::begin() const noexcept {
    return m_data.begin();
  }

  inline auto impact_list::end() const noexcept {
    return m_data.end();
  }

  inline auto impact_list::operator+=(impact a) noexcept
      -> impact_list & {
    m_data.emplace_back(a);
    return *this;
  }

  inline auto impact_list::operator+=(impact_list const &a) noexcept
      -> impact_list & {
    m_data.insert(m_data.end(), a.m_data.begin(), a.m_data.end());
    return *this;
  }

  inline auto operator+(impact a, impact b) noexcept -> impact_list {
    return impact_list { a } += b;
  }

  inline auto operator+(impact_list a, impact b) noexcept
      -> impact_list {
    return a += b;
  }

  inline auto operator+(impact a, impact_list b) noexcept
      -> impact_list {
    return b += a;
  }

  inline auto operator+(impact_list a, impact_list const &b) noexcept
      -> impact_list {
    return a += b;
  }

  inline auto mode_of(impact const &i) noexcept -> mode {
    return std::visit(
        [&](auto const &i) {
          using std::is_same_v, std::decay_t;
          using type = decay_t<decltype(i)>;
          if constexpr (is_same_v<type, tick_continue>)
            return mode::control;
          if constexpr (is_same_v<type, noop> ||
                        is_same_v<type, integer_set> ||
                        is_same_v<type, integer_add> ||
                        is_same_v<type, byte_set> ||
                        is_same_v<type, byte_add>)
            return mode::async;
          return mode::sync;
        },
        i.value);
  }
}

#endif
