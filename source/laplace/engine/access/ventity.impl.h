#pragma once

namespace laplace::engine::access {
  inline ventity::ventity(ventity &&ents) noexcept :
      m_entities(std::move(ents.m_entities)),
      m_mode(std::move(ents.m_mode)) {
    ents.m_mode = forbidden;
  }

  inline ventity::ventity(vptr_entity ents, mode access_mode) :
      m_entities(ents), m_mode(access_mode) { }

  inline auto ventity::operator=(ventity &&ents) noexcept
      -> ventity & {
    m_entities  = std::move(ents.m_entities);
    m_mode      = std::move(ents.m_mode);
    ents.m_mode = forbidden;
    return *this;
  }

  inline auto ventity::size() const -> size_t {
    return m_entities.size();
  }

  inline auto ventity::operator[](size_t i) const -> entity {
    return { m_entities[i], m_mode };
  }
}
