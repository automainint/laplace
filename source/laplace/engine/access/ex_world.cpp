/*  laplace/engine/access/ex_world.cpp
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

#include "../world.h"
#include "world.h"

namespace laplace::engine::access {
  world::world(world &&w) noexcept :
      m_world(std::move(w.m_world)), m_mode(std::move(w.m_mode)) {
    w.m_mode = forbidden;
  }

  world::world(ref_world w, mode access_mode) :
      m_world(w), m_mode(access_mode) { }

  auto world::operator=(world &&w) noexcept -> world & {
    m_world  = std::move(w.m_world);
    m_mode   = std::move(w.m_mode);
    w.m_mode = forbidden;
    return *this;
  }

  void world::desync() {
    if (m_mode != forbidden) {
      m_world.get().desync();
    }
  }

  auto world::reserve(size_t id) -> size_t {
    size_t result = id_undefined;

    if (m_mode == sync) {
      result = m_world.get().reserve(id);
    }

    return result;
  }

  auto world::spawn(ptr_entity ent, size_t id) -> size_t {
    size_t result = id_undefined;

    if (m_mode == sync) {
      result = m_world.get().spawn(ent, id);
    }

    return result;
  }

  void world::remove(size_t id) {
    if (m_mode == sync) {
      m_world.get().remove(id);
    }
  }

  void world::respawn(size_t id) {
    if (m_mode == sync) {
      m_world.get().respawn(id);
    }
  }

  void world::clear() {
    if (m_mode == sync) {
      m_world.get().clear();
    }
  }

  void world::queue(ptr_impact ev) {
    if (m_mode != forbidden) {
      m_world.get().queue(ev);
    }
  }

  void world::set_root(ptr_entity root) {
    if (m_mode == sync) {
      m_world.get().set_root(root);
    }
  }

  auto world::get_root() -> access::entity {
    return { m_world.get().get_root(), m_mode };
  }

  auto world::has_entity(size_t id) -> bool {
    if (m_mode != forbidden) {
      return m_world.get().get_entity(id) ? true : false;
    }

    return false;
  }

  auto world::get_entity(size_t id) -> access::entity {
    return { m_world.get().get_entity(id), m_mode };
  }

  auto world::select(condition op) -> access::ventity {
    return { m_world.get().select(op), m_mode };
  }

  auto world::select_dynamic(condition op) -> access::ventity {
    return { m_world.get().select_dynamic(op), m_mode };
  }

  auto world::get_random_engine() -> ref_rand {
    return m_world.get().get_random();
  }
}
