/*  apps/quadwar/aq_root.cpp
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

#include "../../laplace/core/utils.h"
#include "root.h"
#include "sets.h"
#include <algorithm>

namespace quadwar_app {
  using std::lower_bound, std::unique_lock, std::shared_lock,
      engine::id_undefined, engine::access::entity;

  auto root::changed() -> bool {
    auto _ul = unique_lock(m_lock);

    if (m_is_changed) {
      m_is_changed = false;
      return true;
    }

    return false;
  }

  auto root::get_slot_count() -> size_t {
    auto _sl = shared_lock(m_lock);
    return m_slots.size();
  }

  auto root::get_slot(size_t index) -> size_t {
    auto _sl = shared_lock(m_lock);
    return index < m_slots.size() ? m_slots[index] : id_undefined;
  }

  void root::slot_create(entity en, size_t id_actor) {
    en.modify(sets::slot_create, pack_to_array(id_actor));
  }

  void root::slot_remove(entity en, size_t id_actor) {
    en.modify(sets::slot_remove, pack_to_array(id_actor));
  }

  void root::status_changed(entity en) {
    en.modify(sets::status_changed);
  }

  auto root::get_slot_count(entity en) -> size_t {
    const auto data = en.request(sets::slot_count);
    return rd<size_t>(data, 0);
  }

  auto root::get_slot(entity en, size_t index) -> size_t {
    const auto result = en.request(
        sets::slot_get, pack_to_array(index));

    return rd<size_t>(result, 0);
  }

  auto root::do_request(size_t id, cref_vbyte args) const
      -> vbyte {
    if (id == sets::slot_count) {
      return pack_to_bytes(m_slots.size());
    } else if (id == sets::slot_get) {
      const auto index = rd<size_t>(args, 0);

      if (index < m_slots.size()) {
        return pack_to_bytes(m_slots[index]);
      }
    }

    return {};
  }

  void root::do_modify(size_t id, cref_vbyte args) {
    if (id == sets::status_changed) {
      m_is_changed = true;
    } else if (id == sets::slot_create) {

      const auto id_actor = rd<size_t>(args, 0);
      const auto it       = lower_bound(
          m_slots.begin(), m_slots.end(), id_actor);

      m_slots.emplace(it, id_actor);

      m_is_changed = true;
    } else if (id == sets::slot_remove) {
      const auto id_actor = rd<size_t>(args, 0);
      const auto it       = lower_bound(
          m_slots.begin(), m_slots.end(), id_actor);

      if (it != m_slots.end() && *it == id_actor)
        m_slots.erase(it);

      m_is_changed = true;
    }
  }
}
