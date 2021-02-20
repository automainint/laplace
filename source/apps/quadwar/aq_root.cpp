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

  size_t root::n_is_changed  = 0;
  size_t root::n_is_launched = 0;
  size_t root::n_slot_count  = 0;

  root root::m_proto(root::proto);

  root::root(root::proto_tag) {
    setup_sets( //
        { { .id = sets::root_is_changed, .scale = 1 },
          { .id = sets::root_is_launched, .scale = 1 },
          { .id = sets::root_slot_count, .scale = 1 } });

    n_is_changed  = index_of(sets::root_is_changed);
    n_is_launched = index_of(sets::root_is_launched);
    n_slot_count  = index_of(sets::root_slot_count);
  }

  root::root() {
    *this = m_proto;
  }

  void root::slot_create( //
      entity en,          //
      size_t id_actor) {

    en.modify(sets::root_slot_create, pack_to_array(id_actor));
  }

  void root::slot_remove( //
      entity en,          //
      size_t id_actor) {

    en.modify(sets::root_slot_remove, pack_to_array(id_actor));
  }

  void root::launch(entity en) {
    en.modify(sets::root_launch);
  }

  void root::status_changed(entity en) {
    en.set(n_is_changed, true);
  }

  auto root::changed(entity en) -> bool {
    const auto is_changed = en.get(n_is_changed);
    en.set(n_is_changed, 0);
    return is_changed > 0;
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

  auto root::do_request( //
      size_t     id,     //
      cref_vbyte args) const -> vbyte {

    if (id == sets::root_slot_get) {
      const auto index = rd<size_t>(args, 0);

      if (index < m_slots.size()) {
        return pack_to_bytes(m_slots[index]);
      }
    }

    return {};
  }

  void root::do_modify( //
      size_t     id,    //
      cref_vbyte args) {

    switch (id) {
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

    init(n_slot_count, static_cast<int64_t>(m_slots.size()));
    init(n_is_changed, 1);
  }

  void root::do_slot_remove(const size_t id_actor) {
    const auto it = lower_bound(
        m_slots.begin(), m_slots.end(), id_actor);

    if (it != m_slots.end() && *it == id_actor)
      m_slots.erase(it);

    init(n_slot_count, static_cast<int64_t>(m_slots.size()));
    init(n_is_changed, 1);
  }

  void root::do_launch() {
    if (locked_get(n_is_launched) <= 0) {
      verb("\n ********************\n"
           " *   !! LAUNCH !!   *\n"
           " ********************\n");

      init(n_is_launched, 1);
      init(n_is_changed, 1);
    }
  }
}
