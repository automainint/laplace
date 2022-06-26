/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_IMPACT_IMPL_H
#define LAPLACE_IMPACT_IMPL_H

namespace laplace {
  inline impact_list_intermediate::impact_list_intermediate(
      impact i) noexcept {
    data.emplace_back(i);
  }

  inline auto operator+(impact a, impact b) noexcept
      -> impact_list_intermediate {
    auto list = impact_list_intermediate { a };
    list.data.emplace_back(b);
    return list;
  }

  inline auto operator+(impact_list_intermediate const &a,
                        impact                          b) noexcept
      -> impact_list_intermediate {
    auto list = impact_list_intermediate { a };
    list.data.emplace_back(b);
    return list;
  }

  inline auto operator+(impact                          a,
                        impact_list_intermediate const &b) noexcept
      -> impact_list_intermediate {
    auto list = impact_list_intermediate { a };
    list.data.insert(list.data.end(), b.data.begin(), b.data.end());
    return list;
  }

  inline auto operator+(impact_list_intermediate const &a,
                        impact_list_intermediate const &b) noexcept
      -> impact_list_intermediate {
    auto list = impact_list_intermediate { a };
    list.data.insert(list.data.end(), b.data.begin(), b.data.end());
    return list;
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
}

#endif
