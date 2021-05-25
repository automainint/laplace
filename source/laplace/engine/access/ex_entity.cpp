/*  laplace/engine/access/ex_entity.cpp
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

#include "entity.h"

namespace laplace::engine::access {
  entity::entity(entity &&ent) noexcept :
      m_entity(std::move(ent.m_entity)), m_mode(std::move(ent.m_mode)) {
    ent.m_mode = forbidden;
  }

  entity::entity(ptr_entity ent, mode access_mode) :
      m_entity(ent), m_mode(ent ? access_mode : forbidden) { }

  auto entity::operator=(entity &&ent) noexcept -> entity & {
    m_entity   = std::move(ent.m_entity);
    m_mode     = std::move(ent.m_mode);
    ent.m_mode = forbidden;
    return *this;
  }

  auto entity::exist() const -> bool {
    return m_entity ? true : false;
  }

  void entity::set_dynamic(bool is_dynamic) const {
    if (m_mode > read_only) {
      m_entity->set_dynamic(is_dynamic);
    }
  }

  void entity::set_tick_period(sl::index tick_period) const {
    if (m_mode > read_only) {
      m_entity->set_tick_period(tick_period);
    }
  }

  auto entity::index_of(sl::index id) const -> sl::index {
    return m_mode > forbidden ? m_entity->index_of(id) : id_undefined;
  }

  auto entity::get_count() const -> sl::whole {
    return m_mode > forbidden ? m_entity->get_count() : 0;
  }

  auto entity::id_of(sl::index index) const -> sl::index {
    return m_mode > forbidden ? m_entity->id_of(index) : id_undefined;
  }

  auto entity::scale_of(sl::index index) const -> sl::index {
    return m_mode > forbidden ? m_entity->scale_of(index) : 0;
  }

  auto entity::get(sl::index index, intval defval) const -> intval {
    return m_mode > forbidden ? m_entity->get(index) : defval;
  }

  void entity::set(sl::index index, intval value) const {
    if (m_mode > read_only) {
      m_entity->set(index, value);
    }
  }

  void entity::apply_delta(sl::index index, intval delta) const {
    if (m_mode > read_only) {
      m_entity->apply_delta(index, delta);
    }
  }

  void entity::adjust() const {
    if (m_mode > async) {
      m_entity->adjust();
    }
  }

  auto entity::request(sl::index id, span_cbyte args) const -> vbyte {
    return m_mode > forbidden ? m_entity->request(id, args) : vbyte {};
  }

  void entity::modify(sl::index id, span_cbyte args) const {
    if (m_mode > async) {
      m_entity->modify(id, args);
    }
  }

  auto entity::is_dynamic() const -> bool {
    return m_mode > forbidden ? m_entity->is_dynamic() : false;
  }

  auto entity::get_tick_period() const -> sl::index {
    return m_mode > forbidden ? m_entity->get_tick_period() : 0;
  }

  auto entity::get_id() const -> sl::index {
    return m_mode > forbidden ? m_entity->get_id() : id_undefined;
  }
}
