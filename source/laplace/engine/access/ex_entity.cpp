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
      m_entity(std::move(ent.m_entity)),
      m_mode(std::move(ent.m_mode)) {
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

  void entity::set_dynamic(bool is_dynamic) {
    if (m_mode != forbidden) {
      m_entity->set_dynamic(is_dynamic);
    }
  }

  void entity::set_markable(bool is_markable) {
    if (m_mode != forbidden) {
      m_entity->set_markable(is_markable);
    }
  }

  void entity::set_selectable(bool is_selectable) {
    if (m_mode != forbidden) {
      m_entity->set_selectable(is_selectable);
    }
  }

  void entity::set_vulnerable(bool is_vulnerable) {
    if (m_mode != forbidden) {
      m_entity->set_vulnerable(is_vulnerable);
    }
  }

  void entity::set_tick_period(size_t tick_period) {
    if (m_mode != forbidden) {
      m_entity->set_tick_period(tick_period);
    }
  }

  void entity::set_bounds(eval::cref_box val) {
    if (m_mode != forbidden) {
      m_entity->set_bounds(val);
    }
  }

  auto entity::index_of(size_t id) -> size_t {
    return m_mode != forbidden ? m_entity->index_of(id)
                               : id_undefined;
  }

  auto entity::get_count() -> size_t {
    return m_mode != forbidden ? m_entity->get_count() : 0;
  }

  auto entity::id_of(size_t index) -> size_t {
    return m_mode != forbidden ? m_entity->id_of(index)
                               : id_undefined;
  }

  auto entity::scale_of(size_t index) -> size_t {
    return m_mode != forbidden ? m_entity->scale_of(index) : 0;
  }

  auto entity::get(size_t index) -> int64_t {
    return m_mode != forbidden ? m_entity->get(index) : 0;
  }

  void entity::set(size_t index, int64_t value) {
    if (m_mode != forbidden) {
      m_entity->set(index, value);
    }
  }

  void entity::apply_delta(size_t index, int64_t delta) {
    if (m_mode != forbidden) {
      m_entity->apply_delta(index, delta);
    }
  }

  auto entity::request(size_t id, cref_vbyte args) -> vbyte {
    return m_mode != forbidden ? m_entity->request(id, args)
                               : vbyte {};
  }

  void entity::modify(size_t id, cref_vbyte args) {
    if (m_mode == sync) {
      m_entity->modify(id, args);
    }
  }

  auto entity::is_changed() -> bool {
    if (m_mode == sync) {
      return m_entity->is_changed();
    }

    return false;
  }

  auto entity::is_dynamic() -> bool {
    return m_mode != forbidden ? m_entity->is_dynamic() : false;
  }

  auto entity::is_markable() -> bool {
    return m_mode != forbidden ? m_entity->is_markable()
                               : false;
  }

  auto entity::is_selectable() -> bool {
    return m_mode != forbidden ? m_entity->is_selectable()
                               : false;
  }

  auto entity::is_vulnerable() -> bool {
    return m_mode != forbidden ? m_entity->is_vulnerable()
                               : false;
  }

  auto entity::get_tick_period() -> size_t {
    return m_mode != forbidden ? m_entity->get_tick_period()
                               : 0;
  }

  auto entity::get_bounds() -> eval::box {
    return m_mode != forbidden ? m_entity->get_bounds()
                               : eval::box {};
  }

  auto entity::get_id() -> size_t {
    return m_mode != forbidden ? m_entity->get_id()
                               : id_undefined;
  }
}
