/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "octree.h"

#include <algorithm>
#include <ranges>

namespace laplace::engine::tridi {
  using std::ranges::all_of;

  auto octree::is_empty() const noexcept -> bool {
    if (bounds.is_empty())
      return true;
    if (childs.index() != 0)
      return false;
    return all_of(get<0>(childs),
                  [](auto const &child) { return child.is_empty(); });
  }

  auto octree::contains(vec3i const &point) const noexcept -> bool {
    if (!bounds.contains(point))
      return false;

    if (childs.index() == 0) {
      for (auto const &c : get<0>(childs))
        if (c.contains(point))
          return true;
      return false;
    }

    for (auto const &tri : get<1>(childs))
      if (tri.orientation(point) - epsilon >= 0)
        return false;
    return true;
  }
}
