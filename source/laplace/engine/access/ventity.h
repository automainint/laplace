#pragma once

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
