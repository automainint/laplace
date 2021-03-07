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

  void world::desync() const {
    if (m_mode > read_only) {
      m_world.get().desync();
    }
  }

  auto world::reserve(size_t id) const -> size_t {
    size_t result = id_undefined;

    if (m_mode > async) {
      result = m_world.get().reserve(id);
    }

    return result;
  }

  void world::emplace(ptr_entity ent, size_t id) const {
    if (m_mode > async) {
      m_world.get().emplace(ent, id);
    }
  }

  auto world::spawn(ptr_entity ent, size_t id) const -> size_t {
    size_t result = id_undefined;

    if (m_mode > async) {
      result = m_world.get().spawn(ent, id);
    }

    return result;
  }

  void world::remove(size_t id) const {
    if (m_mode > async) {
      m_world.get().remove(id);
    }
  }

  void world::respawn(size_t id) const {
    if (m_mode > async) {
      m_world.get().respawn(id);
    }
  }

  void world::clear() const {
    if (m_mode > async) {
      m_world.get().clear();
    }
  }

  void world::queue(ptr_impact ev) const {
    if (m_mode > read_only) {
      m_world.get().queue(ev);
    }
  }

  void world::set_root(size_t id_root) const {
    if (m_mode > async) {
      m_world.get().set_root(id_root);
    }
  }

  auto world::get_root() const -> size_t {
    if (m_mode > forbidden) {
      return m_world.get().get_root();
    }

    return id_undefined;
  }

  auto world::has_entity(size_t id) const -> bool {
    if (m_mode > forbidden) {
      return m_world.get().get_entity(id) ? true : false;
    }

    return false;
  }

  auto world::get_entity(size_t id) const -> access::entity {
    return { m_world.get().get_entity(id), m_mode };
  }

  auto world::select(condition op) const -> access::ventity {
    return { m_world.get().select(op), m_mode };
  }

  auto world::select_dynamic(condition op) const -> access::ventity {
    return { m_world.get().select_dynamic(op), m_mode };
  }

  auto world::get_random_engine() const -> ref_rand {
    return m_world.get().get_random();
  }
}
