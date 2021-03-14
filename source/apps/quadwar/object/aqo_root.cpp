/*  apps/quadwar/object/aqo_root.cpp
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

#include "../../../laplace/core/utils.h"
#include "root.h"
#include <algorithm>

namespace quadwar_app::object {
  using std::lower_bound, std::unique_lock, std::shared_lock,
      engine::id_undefined;

  size_t root::n_is_launched = 0;
  size_t root::n_slot_count  = 0;
  size_t root::n_landscape   = 0;

  root root::m_proto(root::proto);

  root::root(root::proto_tag) {
    setup_sets( //
        { { .id = sets::root_is_launched, .scale = 1 },
          { .id = sets::root_slot_count, .scale = 1 },
          { .id = sets::root_landscape, .value = -1 } });

    n_is_launched = index_of(sets::root_is_launched);
    n_slot_count  = index_of(sets::root_slot_count);
    n_landscape   = index_of(sets::root_landscape);
  }

  root::root() : basic_entity(dummy) {
    *this = m_proto;
  }

  void root::slot_create(entity en, size_t id_actor) {

    en.modify(sets::root_slot_create, pack_to_array(id_actor));
  }

  void root::slot_remove(entity en, size_t id_actor) {

    en.modify(sets::root_slot_remove, pack_to_array(id_actor));
  }

  void root::launch(entity en) {
    en.modify(sets::root_launch);
  }

  void root::status_changed(entity en) {
    en.modify(sets::root_changed, pack_to_bytes(true));
  }

  auto root::changed(entity en) -> bool {
    const auto is_changed = rd<bool>(en.request(sets::root_changed), 0);
    if (is_changed)
      en.modify(sets::root_changed, pack_to_bytes(false));
    return is_changed;
  }

  auto root::is_launched(entity en) -> bool {
    return en.get(n_is_launched) > 0;
  }

  auto root::get_slot_count(entity en) -> size_t {
    return static_cast<size_t>(en.get(n_slot_count));
  }

  auto root::get_slot( //
      entity en,       //
      size_t index) -> size_t {

    const auto result = en.request( //
        sets::root_slot_get,        //
        pack_to_array(index));

    return rd<size_t>(result, 0);
  }

  void root::set_landscape(entity en, size_t id_landscape) {
    en.set(n_landscape, static_cast<int64_t>(id_landscape));
  }

  auto root::get_landscape(entity en) -> size_t {
    return static_cast<size_t>(en.get(n_landscape, -1));
  }

  auto root::do_request(size_t id, cref_vbyte args) const -> vbyte {

    if (id == sets::root_changed) {
      return pack_to_bytes(m_is_changed);
    }

    if (id == sets::root_slot_get) {
      const auto index = rd<size_t>(args, 0);

      if (index < m_slots.size()) {
        return pack_to_bytes(m_slots[index]);
      }
    }

    return {};
  }

  void root::do_modify(size_t id, cref_vbyte args) {

    switch (id) {
      case sets::root_changed:
        m_is_changed = rd<bool>(args, 0);
        break;

      case sets::root_slot_create:
        do_slot_create(rd<size_t>(args, 0));
        break;

      case sets::root_slot_remove:
        do_slot_remove(rd<size_t>(args, 0));
        break;

      case sets::root_launch: do_launch(); break;
    }
  }

  void root::do_slot_create(const size_t id_actor) {
    const auto it = lower_bound(
        m_slots.begin(), m_slots.end(), id_actor);

    m_slots.emplace(it, id_actor);

    m_is_changed = true;

    init(n_slot_count, static_cast<int64_t>(m_slots.size()));
  }

  void root::do_slot_remove(const size_t id_actor) {
    const auto it = lower_bound(
        m_slots.begin(), m_slots.end(), id_actor);

    if (it != m_slots.end() && *it == id_actor)
      m_slots.erase(it);

    m_is_changed = true;

    init(n_slot_count, static_cast<int64_t>(m_slots.size()));
  }

  void root::do_launch() {
    if (locked_get(n_is_launched) <= 0) {
      verb(R"(
 ************************************
 *                                  *
 *           L A U N C H            *
 *                                  *
 ************************************
)");

      m_is_changed = true;

      init(n_is_launched, 1);
    }
  }
}
