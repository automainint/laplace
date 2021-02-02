/*  laplace/engine/e_basic_entity.cpp
 *
 *      Basic class for any gameplay object. Implementation.
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

#include "../core/utils.h"
#include "access/world.h"
#include "action/remove.h"
#include "basic_entity.h"
#include "world.h"
#include <algorithm>

using namespace laplace;
using namespace engine;
using namespace object;
using namespace std;

basic_entity::basic_entity(cref_entity en) {
  assign(en);
}

basic_entity::basic_entity(basic_entity &&en) noexcept {
  assign(en);
}

auto basic_entity::operator=(cref_entity en) -> ref_entity {
  assign(en);
  return *this;
}

auto basic_entity::operator=(basic_entity &&en) noexcept
    -> ref_entity {
  assign(en);
  return *this;
}

basic_entity::basic_entity(proto_tag) {
  setup_sets({ { sets::is_dynamic, 1, 0 },
      { sets::is_markable, 1, 0 },
      { sets::is_selectable, 1, 0 },
      { sets::is_vulnerable, 1, 0 },

      { sets::tick_period, sets::scale_time,
          default_tick_period },

      { sets::bounds_min_x, sets::scale_real, +safe_delta },
      { sets::bounds_min_y, sets::scale_real, +safe_delta },
      { sets::bounds_min_z, sets::scale_real, +safe_delta },
      { sets::bounds_max_x, sets::scale_real, -safe_delta },
      { sets::bounds_max_y, sets::scale_real, -safe_delta },
      { sets::bounds_max_z, sets::scale_real, -safe_delta } });
}

basic_entity::basic_entity(dummy_tag) { }

basic_entity::basic_entity(bool is_dynamic, bool is_markable,
    bool is_selectable, bool is_vulnerable,
    uint64_t tick_period, cref_box bounds) {
  setup_sets({ { sets::is_dynamic, 1, is_dynamic },
      { sets::is_markable, 1, is_markable },
      { sets::is_selectable, 1, is_selectable },
      { sets::is_vulnerable, 1, is_vulnerable },

      { sets::tick_period, sets::scale_time,
          static_cast<int64_t>(tick_period) },

      { sets::bounds_min_x, sets::scale_real, bounds.min.x },
      { sets::bounds_min_y, sets::scale_real, bounds.min.y },
      { sets::bounds_min_z, sets::scale_real, bounds.min.z },
      { sets::bounds_max_x, sets::scale_real, bounds.max.x },
      { sets::bounds_max_y, sets::scale_real, bounds.max.y },
      { sets::bounds_max_z, sets::scale_real, bounds.max.z } });
}

void basic_entity::setup_sets(
    basic_entity::cref_vsets_row sets) {
  m_sets.insert(m_sets.end(), sets.begin(), sets.end());

  auto op = [](const sets_row &a, const sets_row &b) -> bool {
    return a.id < b.id;
  };

  sort(m_sets.begin(), m_sets.end(), op);

  for (size_t i = 1; i < m_sets.size();) {
    if (m_sets[i - 1].id == m_sets[i].id) {
      m_sets.erase(m_sets.begin() + i);
    } else {
      i++;
    }
  }
}

void basic_entity::init(size_t index, int64_t value) {
  m_sets[index].value = value;
  m_sets[index].delta = 0;
}

void basic_entity::set_dynamic(bool is_dynamic) {
  auto _ul = unique_lock(m_lock);

  m_sets[n_is_dynamic].delta += is_dynamic ? 1 : -1;
  m_is_changed = true;
}

void basic_entity::set_markable(bool is_markable) {
  auto _ul = unique_lock(m_lock);

  m_sets[n_is_markable].delta += is_markable ? 1 : -1;
  m_is_changed = true;
}

void basic_entity::set_selectable(bool is_selectable) {
  auto _ul = unique_lock(m_lock);

  m_sets[n_is_selectable].delta += is_selectable ? 1 : -1;
  m_is_changed = true;
}

void basic_entity::set_vulnerable(bool is_vulnerable) {
  auto _ul = unique_lock(m_lock);

  m_sets[n_is_vulnerable].delta += is_vulnerable ? 1 : -1;
  m_is_changed = true;
}

void basic_entity::set_tick_period(uint64_t tick_period) {
  auto _ul = unique_lock(m_lock);

  m_sets[n_tick_period].delta +=
      static_cast<int64_t>(tick_period) -
      m_sets[n_tick_period].value;
  m_is_changed = true;
}

void basic_entity::set_clock(uint64_t clock_msec) {
  auto _ul = unique_lock(m_lock);
  m_clock  = clock_msec;
}

void basic_entity::reset_clock() {
  auto _ul = unique_lock(m_lock);
  auto period =
      static_cast<uint64_t>(m_sets[n_tick_period].value);
  m_clock = period - 1;
}

void basic_entity::set_bounds(cref_box val) {
  auto _ul = unique_lock(m_lock);

  m_sets[n_bounds_min_x].delta +=
      val.min.x - m_sets[n_bounds_min_x].value;
  m_sets[n_bounds_min_y].delta +=
      val.min.y - m_sets[n_bounds_min_y].value;
  m_sets[n_bounds_min_z].delta +=
      val.min.z - m_sets[n_bounds_min_z].value;
  m_sets[n_bounds_max_x].delta +=
      val.max.x - m_sets[n_bounds_max_x].value;
  m_sets[n_bounds_max_y].delta +=
      val.max.y - m_sets[n_bounds_max_y].value;
  m_sets[n_bounds_max_z].delta +=
      val.max.z - m_sets[n_bounds_max_z].value;

  m_is_changed = true;
}

void basic_entity::set_id(size_t id) {
  m_id = id;
}

void basic_entity::set_world(ptr_world w) {
  m_world = w;
}

void basic_entity::reset_world() {
  m_world.reset();
}

auto basic_entity::index_of(size_t id) const -> size_t {
  size_t index = m_sets.size();

  auto op = [](const sets_row &row, size_t id) {
    return row.id < id;
  };

  auto it = lower_bound(m_sets.begin(), m_sets.end(), id, op);

  if (it != m_sets.end() && it->id == id) {
    index = as_index(it - m_sets.begin());
  }

  return index;
}

auto basic_entity::get_count() const -> size_t {
  return m_sets.size();
}

auto basic_entity::id_of(size_t index) const -> size_t {
  return index < m_sets.size() ? m_sets[index].id
                               : id_undefined;
}

auto basic_entity::scale_of(size_t index) const -> size_t {
  return index < m_sets.size() ? m_sets[index].scale : 0;
}

auto basic_entity::get(size_t index) -> int64_t {
  auto _sl = shared_lock(m_lock);
  return locked_get(index);
}

void basic_entity::set(size_t index, int64_t value) {
  auto _ul = unique_lock(m_lock);

  if (index < m_sets.size()) {
    m_sets[index].delta += value - m_sets[index].value;

    m_is_changed = true;
  }
}

void basic_entity::apply_delta(size_t index, int64_t delta) {
  auto _ul = unique_lock(m_lock);

  if (index < m_sets.size()) {
    m_sets[index].delta += delta;

    m_is_changed = true;
  }
}

void basic_entity::adjust() {
  auto _ul = unique_lock(m_lock);

  if (m_is_changed) {
    auto is_dynamic_old = m_sets[n_is_dynamic].value > 0;

    for (auto &s : m_sets) {
      s.value += s.delta;
      s.delta = 0;
    }

    /*  Respawn if dynamic status changed.
     */

    if (is_dynamic_old != (m_sets[n_is_dynamic].value > 0)) {
      if (auto w = m_world.lock(); w) {
        w->respawn(m_id);
      }
    }

    m_is_changed = false;
  }
}

auto basic_entity::request(size_t id, cref_vbyte args)
    -> vbyte {
  auto _sl = shared_lock(m_lock);
  return this->do_request(id, args);
}

void basic_entity::modify(size_t id, cref_vbyte args) {
  auto _ul = unique_lock(m_lock);
  this->do_modify(id, args);
}

void basic_entity::tick(access::world w) { }

auto basic_entity::clock() -> bool {
  auto _ul = unique_lock(m_lock);

  bool result = m_clock == 0;
  auto period =
      static_cast<uint64_t>(m_sets[n_tick_period].value);

  if (m_clock == 0) {
    m_clock = period - 1;
  } else if (period > 0) {
    m_clock--;
  }

  return result;
}

auto basic_entity::is_changed() -> bool {
  auto _sl = shared_lock(m_lock);
  return m_is_changed;
}

auto basic_entity::is_dynamic() -> bool {
  auto _sl = shared_lock(m_lock);
  return m_sets[n_is_dynamic].value > 0;
}

auto basic_entity::is_markable() -> bool {
  auto _sl = shared_lock(m_lock);
  return m_sets[n_is_markable].value > 0;
}

auto basic_entity::is_selectable() -> bool {
  auto _sl = shared_lock(m_lock);
  return m_sets[n_is_selectable].value > 0;
}

auto basic_entity::is_vulnerable() -> bool {
  auto _sl = shared_lock(m_lock);
  return m_sets[n_is_vulnerable].value > 0;
}

auto basic_entity::get_tick_period() -> uint64_t {
  auto _sl = shared_lock(m_lock);
  return static_cast<uint64_t>(m_sets[n_tick_period].value);
}

auto basic_entity::get_bounds() -> box {
  auto _sl = shared_lock(m_lock);

  return box { { m_sets[n_bounds_min_x].value,
                   m_sets[n_bounds_min_y].value,
                   m_sets[n_bounds_min_z].value },
    { m_sets[n_bounds_max_x].value,
        m_sets[n_bounds_max_y].value,
        m_sets[n_bounds_max_z].value } };
}

auto basic_entity::get_id() const -> size_t {
  return m_id;
}

auto basic_entity::locked_get(size_t index) const -> int64_t {
  return index < m_sets.size() ? m_sets[index].value : 0;
}

void basic_entity::self_destruct(access::world w) {
  w.queue(make_shared<action::remove>(m_id));
}

auto basic_entity::do_request(size_t id, cref_vbyte args) const
    -> vbyte {
  return {};
}

void basic_entity::do_modify(size_t id, cref_vbyte args) { }

void basic_entity::assign(cref_entity en) {
  m_is_changed = en.m_is_changed;
  m_id         = en.m_id;

  m_sets  = en.m_sets;
  m_clock = en.m_clock;
  m_world = en.m_world;
}

void basic_entity::assign(basic_entity &&en) {
  m_is_changed = move(en.m_is_changed);
  m_id         = move(en.m_id);

  m_sets  = move(en.m_sets);
  m_clock = move(en.m_clock);
  m_world = move(en.m_world);
}
