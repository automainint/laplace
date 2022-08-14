#ifndef LAPLACE_X_IMPACT_IMPL_H
#define LAPLACE_X_IMPACT_IMPL_H

namespace laplace {
  inline impact::impact(noop const &i) noexcept : value(i) { }
  inline impact::impact(integer_set const &i) noexcept : value(i) { }
  inline impact::impact(integer_add const &i) noexcept : value(i) { }
  inline impact::impact(byte_set const &i) noexcept : value(i) { }
  inline impact::impact(byte_add const &i) noexcept : value(i) { }
  inline impact::impact(integer_reserve const &i) noexcept :
      value(i) { }
  inline impact::impact(integer_allocate_into const &i) noexcept :
      value(i) { }
  inline impact::impact(integer_allocate const &i) noexcept :
      value(i) { }
  inline impact::impact(integer_deallocate const &i) noexcept :
      value(i) { }
  inline impact::impact(byte_reserve const &i) noexcept : value(i) { }
  inline impact::impact(byte_allocate_into const &i) noexcept :
      value(i) { }
  inline impact::impact(byte_allocate const &i) noexcept :
      value(i) { }
  inline impact::impact(byte_deallocate const &i) noexcept :
      value(i) { }
  inline impact::impact(integer_seed const &i) noexcept : value(i) { }
  inline impact::impact(integer_random const &i) noexcept :
      value(i) { }
  inline impact::impact(queue_action const &i) noexcept : value(i) { }
  inline impact::impact(tick_continue const &i) noexcept :
      value(i) { }

  inline impact_list::impact_list() noexcept :
      m_data(default_memory_resource()) {
    m_data.reserve(average_impact_list_size());
  }

  inline impact_list::impact_list(
      ptrdiff_t reserve, std::pmr::memory_resource *resource) noexcept
      :
      m_data(resource) {
    m_data.reserve(reserve);
  }

  inline impact_list::impact_list(
      impact i, std::pmr::memory_resource *resource) noexcept :
      m_data(resource) {
    _assign(std::move(i));
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

  inline void impact_list::clear() noexcept {
    m_data.clear();
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

  inline void impact_list::_assign(impact i) noexcept {
    m_data.reserve(average_impact_list_size());
    m_data.emplace_back(i);
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

  template <typename impact_type>
  constexpr auto mode_of() noexcept -> mode {
    using std::is_same_v;
    if constexpr (is_same_v<impact_type, tick_continue>)
      return mode::control;
    if constexpr (is_same_v<impact_type, noop> ||
                  is_same_v<impact_type, integer_set> ||
                  is_same_v<impact_type, integer_add> ||
                  is_same_v<impact_type, byte_set> ||
                  is_same_v<impact_type, byte_add>)
      return mode::async;
    return mode::sync;
  }

  inline auto mode_of(impact const &i) noexcept -> mode {
    return std::visit(
        [&](auto const &i) {
          using std::is_same_v, std::decay_t;
          using type = decay_t<decltype(i)>;
          return mode_of<type>();
        },
        i.value);
  }
}

#endif
