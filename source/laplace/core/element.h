/*  laplace/core/element.h
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

#ifndef laplace_core_element_h
#define laplace_core_element_h

#include "defs.h"
#include <atomic>
#include <memory>

namespace laplace::core {
  /*  Base class for any object, that can be loaded from a file.
   */
  class element {
  public:
    std::wstring file_name = L"";

    element() noexcept          = default;
    virtual ~element() noexcept = default;

    [[nodiscard]] auto get_type_id() const noexcept -> sl::index;

    [[nodiscard]] static auto type_id() noexcept -> sl::index;

  protected:
    void set_type_id(sl::index id) noexcept;

    class type {
    public:
      type() noexcept;
      ~type() noexcept = default;

      [[nodiscard]] auto get_id() const noexcept -> sl::index;

    private:
      static std::atomic<sl::index> m_count;

      sl::index m_id = 0;
    };

  private:
    sl::index m_type_id = type_id();
  };

  using ptr_element  = std::shared_ptr<element>;
  using vptr_element = std::vector<ptr_element>;
}

#endif
