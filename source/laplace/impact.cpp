/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "impact.h"

namespace laplace {
  using std::pmr::synchronized_pool_resource;

  static auto impact_resource = synchronized_pool_resource {};

  impact_list::impact_list(impact i) noexcept :
      m_data(&impact_resource) {
    m_data.emplace_back(i);
  }

  auto impact_list::get_size() const noexcept -> ptrdiff_t {
    return static_cast<ptrdiff_t>(m_data.size());
  }

  auto impact_list::add(impact i) const noexcept -> impact_list {
    auto result = impact_list { *this };
    result.m_data.emplace_back(i);
    return result;
  }

  auto impact_list::add(impact_list i) const noexcept -> impact_list {
    auto result = impact_list { *this };
    result.m_data.insert(result.m_data.end(), i.m_data.begin(),
                         i.m_data.end());
    return result;
  }

  auto impact_list::operator[](ptrdiff_t index) const noexcept
      -> impact const & {
    return m_data[index];
  }

  auto operator+(impact a, impact b) noexcept -> impact_list {
    return impact_list { a }.add(b);
  }

  auto operator+(impact_list const &a, impact b) noexcept
      -> impact_list {
    return a.add(b);
  }

  auto operator+(impact a, impact_list const &b) noexcept
      -> impact_list {
    return impact_list { a }.add(b);
  }

  auto operator+(impact_list const &a, impact_list const &b) noexcept
      -> impact_list {
    return a.add(b);
  }
}