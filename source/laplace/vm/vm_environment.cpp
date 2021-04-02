/*  laplace/network/vm_environment.cpp
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "agent.h"
#include "environment.h"
#include <algorithm>

namespace laplace::vm {
  using std::pair;

  auto environment::add(size_t unique_id, ptr_agent a) -> size_t {
    size_t result = m_ids.size();

    m_ids.emplace_back(pair { unique_id, a });

    return result;
  }

  auto environment::get_id(size_t unique_id) -> size_t {
    size_t id = 0;

    for (; id < m_ids.size(); id++) {
      if (m_ids[id].first == unique_id)
        break;
    }

    return id;
  }

  auto environment::perform(size_t id, span_cbyte args) -> vbyte {
    vbyte result;

    if (id < m_ids.size()) {
      auto &a = m_ids[id];

      result = a.second->perform(a.first, args);
    }

    return result;
  }
}
