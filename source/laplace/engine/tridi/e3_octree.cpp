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

namespace laplace::engine::tridi {
  using std::ranges::all_of, std::ranges::any_of;

  auto octree::is_empty() const noexcept -> bool {
    if (bounds.is_empty())
      return true;
    if (children.index() != n_octrees)
      return get<1>(children).empty();
    return all_of(get<n_octrees>(children),
                  [](auto const &child) { return child.is_empty(); });
  }

  auto octree::contains(vec3i const &point) const noexcept -> bool {
    if (!bounds.contains(point))
      return false;

    if (children.index() == n_octrees)
      return any_of(get<n_octrees>(children), [&](auto const &child) {
        return child.contains(point);
      });

    return any_of(get<n_triangles>(children), [&](auto const &tri) {
      return tri.orientation(point) - epsilon >= 0;
    });
  }
}
