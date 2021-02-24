/*  laplace/engine/access/ventity.h
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

#ifndef laplace_engine_access_ventity_h
#define laplace_engine_access_ventity_h

#include "entity.h"

namespace laplace::engine::access {
  class ventity {
  public:
    ventity(const ventity &) = delete;
    auto operator=(const ventity &) -> ventity & = delete;

    [[nodiscard]] ventity(ventity &&ents) noexcept;
    [[nodiscard]] ventity(vptr_entity ents, mode access_mode);

    auto operator=(ventity &&ents) noexcept -> ventity &;

    ~ventity() = default;

    auto size() const -> size_t;
    auto operator[](size_t i) const -> entity;

  private:
    vptr_entity m_entities;
    mode        m_mode = forbidden;
  };
}

#include "ventity.impl.h"

#endif
