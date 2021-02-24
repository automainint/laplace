/*  laplace/core/object_manager.impl.h
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

#ifndef laplace_core_object_manager_h
#define laplace_core_object_manager_h

namespace laplace::core {
  template <typename type>
  inline auto object_manager::load(std::wstring_view file_name)
      -> std::shared_ptr<type> {
    std::shared_ptr<type> result;

    auto p = this->load_raw(file_name);

    if (p && p->get_type_id() == type::type_id()) {
      result = std::reinterpret_pointer_cast<type>(p);
    }

    return result;
  }
}

#endif
