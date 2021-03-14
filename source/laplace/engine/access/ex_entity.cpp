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

  void entity::set_markable(bool is_markable) const {
    if (m_mode > read_only) {
      m_entity->set_markable(is_markable);
    }
  }

  void entity::set_selectable(bool is_selectable) const {
    if (m_mode > read_only) {
      m_entity->set_selectable(is_selectable);
    }
  }

  void entity::set_vulnerable(bool is_vulnerable) const {
    if (m_mode > read_only) {
      m_entity->set_vulnerable(is_vulnerable);
    }
  }

  void entity::set_tick_period(size_t tick_period) const {
    if (m_mode > read_only) {
      m_entity->set_tick_period(tick_period);
    }
  }

  void entity::set_bounds(eval::cref_box val) const {
    if (m_mode > read_only) {
      m_entity->set_bounds(val);
    }
  }

  auto entity::index_of(size_t id) const -> size_t {
    return m_mode > forbidden ? m_entity->index_of(id) : id_undefined;
  }

  auto entity::get_count() const -> size_t {
    return m_mode > forbidden ? m_entity->get_count() : 0;
  }

  auto entity::id_of(size_t index) const -> size_t {
    return m_mode > forbidden ? m_entity->id_of(index) : id_undefined;
  }

  auto entity::scale_of(size_t index) const -> size_t {
    return m_mode > forbidden ? m_entity->scale_of(index) : 0;
  }

  auto entity::get(size_t index, int64_t defval) const -> int64_t {
    return m_mode > forbidden ? m_entity->get(index) : defval;
  }

  void entity::set(size_t index, int64_t value) const {
    if (m_mode > read_only) {
      m_entity->set(index, value);
    }
  }

  void entity::apply_delta(size_t index, int64_t delta) const {
    if (m_mode > read_only) {
      m_entity->apply_delta(index, delta);
    }
  }

  void entity::adjust() const {
    if (m_mode > async) {
      m_entity->adjust();
    }
  }

  auto entity::request(size_t id, cref_vbyte args) const -> vbyte {
    return m_mode > forbidden ? m_entity->request(id, args) : vbyte {};
  }

  void entity::modify(size_t id, cref_vbyte args) const {
    if (m_mode > async) {
      m_entity->modify(id, args);
    }
  }

  auto entity::is_changed() const -> bool {
    if (m_mode > forbidden) {
      return m_entity->is_changed();
    }

    return false;
  }

  auto entity::is_dynamic() const -> bool {
    return m_mode > forbidden ? m_entity->is_dynamic() : false;
  }

  auto entity::is_markable() const -> bool {
    return m_mode > forbidden ? m_entity->is_markable() : false;
  }

  auto entity::is_selectable() const -> bool {
    return m_mode > forbidden ? m_entity->is_selectable() : false;
  }

  auto entity::is_vulnerable() const -> bool {
    return m_mode > forbidden ? m_entity->is_vulnerable() : false;
  }

  auto entity::get_tick_period() const -> size_t {
    return m_mode > forbidden ? m_entity->get_tick_period() : 0;
  }

  auto entity::get_bounds() const -> eval::box {
    return m_mode > forbidden ? m_entity->get_bounds() : eval::box {};
  }

  auto entity::get_id() const -> size_t {
    return m_mode > forbidden ? m_entity->get_id() : id_undefined;
  }
}
