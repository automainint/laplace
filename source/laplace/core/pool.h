/*  laplace/core/pool.h
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

#ifndef laplace_core_pool_h
#define laplace_core_pool_h

#include "element.h"
#include <shared_mutex>

namespace laplace::core {
  /*  Object pool of elements.
   */
  class pool {
  public:
    void clear();
    void store(std::wstring_view file_name, ptr_element elem);
    void release(std::wstring_view file_name);
    auto load(std::wstring_view file_name) -> ptr_element;
    auto contains(std::wstring_view file_name) -> bool;

  private:
    /*  Comparison operator for lower bound.
     */
    static auto op(const ptr_element &a, std::wstring_view b)
        -> bool;

    std::shared_mutex m_lock;
    vptr_element      m_elems;
  };

  using cref_pool = const pool &;
  using ref_pool  = pool &;
  using ptr_pool  = std::shared_ptr<pool>;
  using vptr_pool = std::vector<ptr_pool>;
}

#endif
