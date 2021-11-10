/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "root.h"

#include "../../../laplace/core/utils.h"

namespace quadwar_app::object {
  using std::make_shared, engine::id_undefined;

  sl::index root::n_version     = {};
  sl::index root::n_is_loading  = {};
  sl::index root::n_is_launched = {};
  sl::index root::n_landscape   = {};
  sl::index root::n_pathmap     = {};
  sl::index root::n_slots       = {};
  sl::index root::n_units       = {};

  root root::m_proto { root::proto };

  root::root(root::proto_tag) noexcept {
    setup_sets({ { .id = sets::state_version, .scale = 1 },
                 { .id = sets::root_is_loading, .scale = 1 },
                 { .id = sets::root_is_launched, .scale = 1 },
                 { .id = sets::root_landscape, .value = -1 },
                 { .id = sets::root_pathmap, .value = -1 },
                 { .id = sets::root_slots, .value = -1 },
                 { .id = sets::root_units, .value = -1 } });

    n_version     = index_of(sets::state_version);
    n_is_loading  = index_of(sets::root_is_loading);
    n_is_launched = index_of(sets::root_is_launched);
    n_landscape   = index_of(sets::root_landscape);
    n_pathmap     = index_of(sets::root_pathmap);
    n_slots       = index_of(sets::root_slots);
    n_units       = index_of(sets::root_units);
  }

  root::root() noexcept : basic_entity(dummy) {
    *this = m_proto;
  }

  void root::create(world w) {
    w.set_root(w.spawn(make_shared<root>(), id_undefined));

    auto r = w.get_entity(w.get_root());

    r.set(n_slots,
          w.spawn(make_shared<basic_entity>(), id_undefined));
    r.set(n_units,
          w.spawn(make_shared<basic_entity>(), id_undefined));
    r.adjust();
  }

  void root::loading(entity en) {
    en.set(n_is_loading, 1);
    en.apply_delta(n_version, 1);
    en.adjust();
  }

  void root::launch(entity en) {
    en.set(n_is_launched, 1);
    en.apply_delta(n_version, 1);
    en.adjust();
  }

  void root::status_changed(entity en) {
    en.apply_delta(n_version, 1);
    en.adjust();
  }

  void root::set_landscape(entity en, sl::index id_landscape) {
    en.set(n_landscape, static_cast<int64_t>(id_landscape));
  }

  void root::set_pathmap(entity en, sl::index id_pathmap) {
    en.set(n_pathmap, static_cast<int64_t>(id_pathmap));
  }

  auto root::get_version(entity en) -> sl::index {
    return static_cast<sl::index>(en.get(n_version));
  }

  auto root::is_loading(entity en) -> bool {
    return en.get(n_is_loading) > 0;
  }

  auto root::is_launched(entity en) -> bool {
    return en.get(n_is_launched) > 0;
  }

  auto root::get_landscape(entity en) -> sl::index {
    return as_index(en.get(n_landscape, -1));
  }

  auto root::get_pathmap(entity en) -> sl::index {
    return as_index(en.get(n_pathmap, -1));
  }

  auto root::get_slots(entity en) -> sl::index {
    return as_index(en.get(n_slots, -1));
  }

  auto root::get_units(entity en) -> sl::index {
    return as_index(en.get(n_units, -1));
  }
}
