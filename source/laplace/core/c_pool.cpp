/*  laplace/core/c_pool.cpp
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

#include "pool.h"
#include <algorithm>
#include <filesystem>

namespace laplace::core {
  using namespace std;

  void pool::clear() {
    auto _ul = unique_lock(m_lock);
    m_elems.clear();
  }

  void pool::store(wstring_view file_name, ptr_element elem) {
    if (elem) {
      elem->file_name = file_name;

      auto _ul = unique_lock(m_lock);

      auto i = lower_bound(
          m_elems.begin(), m_elems.end(), file_name, op);

      if (i == m_elems.end() || (*i)->file_name != file_name) {
        m_elems.emplace(i, elem);
      } else {
        *i = elem;
      }
    }
  }

  void pool::release(wstring_view file_name) {
    auto _ul = unique_lock(m_lock);

    auto i = lower_bound(
        m_elems.begin(), m_elems.end(), file_name, op);

    if (i != m_elems.end() && (*i)->file_name == file_name) {
      m_elems.erase(i);
    }
  }

  auto pool::load(wstring_view file_name) -> ptr_element {
    ptr_element result;

    auto _sl = shared_lock(m_lock);

    auto i = lower_bound(
        m_elems.begin(), m_elems.end(), file_name, op);

    if (i != m_elems.end() && (*i)->file_name == file_name) {
      result = *i;
    }

    return result;
  }

  auto pool::contains(wstring_view file_name) -> bool {
    auto _sl = shared_lock(m_lock);

    auto i = lower_bound(
        m_elems.begin(), m_elems.end(), file_name, op);

    return i != m_elems.end() && (*i)->file_name == file_name;
  }

  auto pool::op(const ptr_element &a, wstring_view b) -> bool {
    return a->file_name < b;
  }
}
