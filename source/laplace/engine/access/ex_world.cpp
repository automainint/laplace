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

#include "world.h"

#include "../world.h"

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
    if (is_allowed(async, m_mode)) {
      m_world.get().desync();
    }
  }

  auto world::reserve(sl::index id) const -> sl::index {
    if (is_allowed(sync, m_mode)) {
      return m_world.get().reserve(id);
    }

    return id_undefined;
  }

  void world::emplace(ptr_entity ent, sl::index id) const {
    if (is_allowed(sync, m_mode)) {
      m_world.get().emplace(ent, id);
    }
  }

  auto world::spawn(ptr_entity ent, sl::index id) const -> sl::index {
    if (is_allowed(sync, m_mode)) {
      return m_world.get().spawn(ent, id);
    }

    return id_undefined;
  }

  void world::remove(sl::index id) const {
    if (is_allowed(sync, m_mode)) {
      m_world.get().remove(id);
    }
  }

  void world::respawn(sl::index id) const {
    if (is_allowed(sync, m_mode)) {
      m_world.get().respawn(id);
    }
  }

  void world::clear() const {
    if (is_allowed(sync, m_mode)) {
      m_world.get().clear();
    }
  }

  void world::queue(ptr_impact ev) const {
    if (is_allowed(async, m_mode)) {
      m_world.get().queue(ev);
    }
  }

  void world::set_root(sl::index id_root) const {
    if (is_allowed(sync, m_mode)) {
      m_world.get().set_root(id_root);
    }
  }

  auto world::get_root() const -> sl::index {
    if (is_allowed(read_only, m_mode)) {
      return m_world.get().get_root();
    }

    return id_undefined;
  }

  auto world::has_entity(sl::index id) const -> bool {
    if (is_allowed(read_only, m_mode)) {
      return m_world.get().get_entity(id) ? true : false;
    }

    return false;
  }

  auto world::get_entity(sl::index id) const -> access::entity {
    return { m_world.get().get_entity(id), m_mode };
  }

  auto world::get_random_engine() const -> ref_rand {
    return m_world.get().get_random();
  }
}
