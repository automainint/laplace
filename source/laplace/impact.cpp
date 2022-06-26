/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "impact.h"

namespace laplace {
  template <class... types_>
  struct overload : types_... {
    using types_::operator()...;
  };

  template <class... types_>
  overload(types_...) -> overload<types_...>;

  impact_list::impact_list(impact i) noexcept {
    data = i;
    size = 1;
  }

  auto impact_list::get_size() const noexcept -> ptrdiff_t {
    return size;
  }

  auto impact_list::add(impact i) const noexcept -> impact_list {
    auto result = impact_list {};
    result.size = size + 1;
    if (result.size == 1)
      result.data = i;
    else if (result.size <= small_size) {
      result.data = small_data {};
      ptrdiff_t k = 0;
      for (; k < get_size(); k++)
        std::get<1>(result.data).v[k] = (*this)[k];
      std::get<1>(result.data).v[k] = i;
    }
    return result;
  }

  auto impact_list::operator[](ptrdiff_t index) const noexcept
      -> impact const & {
    return std::visit(
        overload {
            [&](impact const &i) -> impact const & { return i; },
            [&](small_data const &v) -> impact const & {
              return v.v[index];
            },
            [&](large_data const &v) -> impact const & {
              return v.v[index];
            } },
        data);
  }

  auto operator+(impact a, impact b) noexcept -> impact_list {
    return impact_list { a }.add(b);
  }
}