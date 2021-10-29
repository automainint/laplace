/*  laplace/engine/e_basic_entity.cpp
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

#include "basic_entity.h"

#include "../core/utils.h"
#include "access/world.h"
#include "action/remove.h"
#include "world.h"
#include <algorithm>

namespace laplace::engine {
  namespace sets = object::sets;

  using std::unique_lock, std::shared_lock, std::span, std::min,
      std::max, std::move, std::make_unique, std::lower_bound,
      std::chrono::milliseconds;

  const milliseconds basic_entity::lock_timeout = milliseconds(300);
  const sl::time     basic_entity::default_tick_period = 10;

  basic_entity::basic_entity(cref_entity en) noexcept {
    assign(en);
  }

  basic_entity::basic_entity(basic_entity &&en) noexcept {
    assign(en);
  }

  auto basic_entity::operator=(cref_entity en) noexcept
      -> ref_entity {

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
                 { sets::tick_period, sets::scale_time,
                   default_tick_period } });
  }

  basic_entity::basic_entity(dummy_tag) { }

  basic_entity::basic_entity(dynamic_tag, sl::time tick_period) {

    setup_sets({ { sets::is_dynamic, 1, 1 },
                 { sets::tick_period, sets::scale_time,
                   static_cast<intval>(tick_period) } });
  }

  void basic_entity::set_dynamic(bool is_dynamic) {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      m_sets[n_is_dynamic].delta += is_dynamic ? 1 : -1;
      sets_invalidate(n_is_dynamic);
    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::set_tick_period(sl::time tick_period) {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      m_sets[n_tick_period].delta += static_cast<int64_t>(
                                         tick_period) -
                                     m_sets[n_tick_period].value;

      sets_invalidate(n_tick_period);
    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::set_clock(sl::time clock_msec) {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      m_clock = clock_msec;
    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::reset_clock() {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      const auto period = m_sets[n_tick_period].value;
      m_clock           = period - 1;
    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::set_world(ptr_world w, sl::index id) {
    m_world = w;
    m_id    = id;
  }

  void basic_entity::reset_world() {
    m_world.reset();
    m_id = id_undefined;
  }

  auto basic_entity::index_of(sl::index id) -> sl::index {
    if (auto const it = lower_bound(
            m_sets.begin(), m_sets.end(), id,
            [](sets_row const &row, sl::index const id) {
              return row.id < id;
            });
        it != m_sets.end() && it->id == id) {
      return as_index(it - m_sets.begin());
    }

    error_("Invalid id.", __FUNCTION__);
    desync();
    return id_undefined;
  }

  auto basic_entity::get_count() const -> sl::whole {
    return m_sets.size();
  }

  auto basic_entity::id_of(sl::index n) -> sl::index {
    if (n < 0 || n >= m_sets.size()) {
      error_("Invalid index.", __FUNCTION__);
      desync();
      return id_undefined;
    }

    return m_sets[n].id;
  }

  auto basic_entity::scale_of(sl::index n) -> sl::index {
    if (n < 0 || n >= m_sets.size()) {
      error_("Invalid index.", __FUNCTION__);
      desync();
      return 0;
    }

    return m_sets[n].scale;
  }

  auto basic_entity::get(sl::index n) -> intval {
    if (auto _sl = shared_lock(m_lock, lock_timeout); _sl) {
      if (n < 0 || n >= m_sets.size()) {
        error_("Invalid index.", __FUNCTION__);
        desync();
        return {};
      }

      return m_sets[n].value;
    }

    error_("Lock timeout.", __FUNCTION__);
    desync();
    return {};
  }

  void basic_entity::set(sl::index n, intval value) {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      if (n < 0 || n >= m_sets.size()) {
        error_("Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      m_sets[n].delta += value - m_sets[n].value;
      sets_invalidate(n);

    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::apply_delta(sl::index n, intval delta) {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      if (n < 0 || n >= m_sets.size()) {
        error_("Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      m_sets[n].delta += delta;
      sets_invalidate(n);

    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  auto basic_entity::bytes_get_size() noexcept -> sl::whole {
    if (auto _sl = shared_lock(m_lock, lock_timeout); _sl) {
      return m_bytes.size();
    }

    error_("Lock timeout.", __FUNCTION__);
    desync();
    return {};
  }

  auto basic_entity::bytes_get(sl::index n) noexcept -> int8_t {
    if (auto _sl = shared_lock(m_lock, lock_timeout); _sl) {
      if (n < 0 || n >= m_bytes.size()) {
        error_("Invalid index.", __FUNCTION__);
        desync();
        return {};
      }

      return m_bytes[n].value;
    }

    error_("Lock timeout.", __FUNCTION__);
    desync();
    return {};
  }

  auto basic_entity::bytes_get_all() noexcept -> sl::vector<int8_t> {
    if (auto _sl = shared_lock(m_lock, lock_timeout); _sl) {
      auto v = sl::vector<int8_t>(m_bytes.size());

      for (sl::index i = 0; i < v.size(); i++) {
        v[i] = m_bytes[i].value;
      }

      return v;
    }

    error_("Lock timeout.", __FUNCTION__);
    desync();
    return {};
  }

  void basic_entity::bytes_read(sl::index    n,
                                span<int8_t> dst) noexcept {
    if (auto _sl = shared_lock(m_lock, lock_timeout); _sl) {
      if (n + dst.size() > m_bytes.size()) {
        error_("Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      for (sl::index i = 0; i < dst.size(); i++) {
        dst.begin()[i] = m_bytes[n + i].value;
      }

    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::bytes_set(sl::index n, int8_t value) noexcept {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      if (n < 0 || n >= m_bytes.size()) {
        error_("Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      m_bytes[n].delta += value - m_bytes[n].value;
      bytes_invalidate(n);

    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::bytes_write(sl::index          n,
                                 span<const int8_t> values) noexcept {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      if (n < 0 || n + values.size() > m_bytes.size()) {
        error_("Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      for (sl::index i = 0; i < values.size(); i++) {
        m_bytes[n + i].delta += values[i] - m_bytes[n + i].value;
      }

      bytes_invalidate(n, values.size());

    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::bytes_apply_delta(sl::index n,
                                       int8_t    delta) noexcept {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      if (n < 0 || n >= m_bytes.size()) {
        error_("Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      m_bytes[n].delta += delta;
      bytes_invalidate(n);

    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::bytes_write_delta(
      sl::index n, span<const int8_t> deltas) noexcept {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      if (n < 0 || n + deltas.size() > m_bytes.size()) {
        error_("Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      for (sl::index i = 0; i < deltas.size(); i++) {
        m_bytes[n + i].delta += deltas[i];
      }

      bytes_invalidate(n, deltas.size());

    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::bytes_erase_delta(
      sl::index n, span<const int8_t> deltas) noexcept {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      if (n < 0 || n + deltas.size() > m_bytes.size()) {
        error_("Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      for (sl::index i = 0; i < deltas.size(); i++) {
        m_bytes[n + i].delta -= deltas[i];
      }

      bytes_invalidate(n, deltas.size());

    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::bytes_resize(sl::whole size) noexcept {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      if (size < 0) {
        error_("Invalid size.", __FUNCTION__);
        desync();
        return;
      }

      m_bytes.resize(size);

    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  auto basic_entity::vec_get_size() noexcept -> sl::whole {
    if (auto _sl = shared_lock(m_lock, lock_timeout); _sl) {
      return m_vec.size();
    }

    error_("Lock timeout.", __FUNCTION__);
    desync();
    return {};
  }

  auto basic_entity::vec_get(sl::index n) noexcept -> intval {
    if (auto _sl = shared_lock(m_lock, lock_timeout); _sl) {
      if (n < 0 || n >= m_vec.size()) {
        error_("Invalid index.", __FUNCTION__);
        desync();
        return {};
      }

      return m_vec[n].value;
    }

    error_("Lock timeout.", __FUNCTION__);
    desync();
    return {};
  }

  auto basic_entity::vec_get_all() noexcept -> sl::vector<intval> {
    if (auto _sl = shared_lock(m_lock, lock_timeout); _sl) {
      auto v = sl::vector<intval>(m_vec.size());

      for (sl::index i = 0; i < v.size(); i++) {
        v[i] = m_vec[i].value;
      }

      return v;
    }

    error_("Lock timeout.", __FUNCTION__);
    desync();
    return {};
  }

  void basic_entity::vec_set(sl::index n, intval value) noexcept {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      if (n < 0 || n >= m_vec.size()) {
        error_("Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      m_vec[n].delta += value - m_vec[n].value;
      vec_invalidate(n);

    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::vec_apply_delta(sl::index n,
                                     intval    delta) noexcept {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      if (n < 0 || n >= m_vec.size()) {
        error_("Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      m_vec[n].delta += delta;
      vec_invalidate(n);

    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::vec_read(sl::index    n,
                              span<intval> dst) noexcept {
    if (auto _sl = shared_lock(m_lock, lock_timeout); _sl) {
      if (n < 0 || n + dst.size() > m_vec.size()) {
        error_("Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      for (sl::index i = 0; i < dst.size(); i++) {
        dst[i] = m_vec[n + i].value;
      }
    }

    error_("Lock timeout.", __FUNCTION__);
    desync();
    return;
  }

  void basic_entity::vec_write(sl::index          n,
                               span<const intval> values) noexcept {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      if (n < 0 || n + values.size() > m_vec.size()) {
        error_("Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      for (sl::index i = 0; i < values.size(); i++) {
        m_vec[n + i].delta += values[i] - m_vec[n + i].value;
      }

      vec_invalidate(n, values.size());
    }

    error_("Lock timeout.", __FUNCTION__);
    desync();
    return;
  }

  void basic_entity::vec_write_delta(
      sl::index n, span<const intval> deltas) noexcept {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      if (n < 0 || n + deltas.size() > m_vec.size()) {
        error_("Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      for (sl::index i = 0; i < deltas.size(); i++) {
        m_vec[n + i].delta += deltas[i];
      }

      vec_invalidate(n, deltas.size());
    }

    error_("Lock timeout.", __FUNCTION__);
    desync();
    return;
  }

  void basic_entity::vec_erase_delta(
      sl::index n, span<const intval> deltas) noexcept {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      if (n < 0 || n + deltas.size() > m_vec.size()) {
        error_("Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      for (sl::index i = 0; i < deltas.size(); i++) {
        m_vec[n + i].delta -= deltas[i];
      }

      vec_invalidate(n, deltas.size());
    }

    error_("Lock timeout.", __FUNCTION__);
    desync();
    return;
  }

  void basic_entity::vec_resize(sl::whole size) noexcept {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      if (size < 0) {
        error_("Invalid size.", __FUNCTION__);
        desync();
        return;
      }

      m_vec.resize(size);

    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::vec_add(intval value) noexcept {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {

      m_vec.emplace_back(vec_row { value, 0 });

    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::vec_add_sorted(intval value) noexcept {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {

      const auto i = lower_bound(
          m_vec.begin(), m_vec.end(), value,
          [](const vec_row &a, const intval b) -> bool {
            return a.value < b;
          });

      m_vec.insert(i, vec_row { value, 0 });

    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::vec_insert(sl::index n, intval value) noexcept {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      if (n < 0 || n > m_vec.size()) {
        error_("Invalid idnex.", __FUNCTION__);
        desync();
        return;
      }

      m_vec.insert(m_vec.begin() + n, vec_row { value, 0 });

    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::vec_erase(sl::index n) noexcept {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      if (n < 0 || n >= m_vec.size()) {
        error_("Invalid idnex.", __FUNCTION__);
        desync();
        return;
      }

      m_vec.erase(m_vec.begin() + n);

    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::vec_erase_by_value(intval value) noexcept {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {

      auto _erase = [&]() {
        for (auto i = m_vec.begin(); i != m_vec.end(); i++) {
          if (i->value == value) {
            m_vec.erase(i);
            return false;
          }
        }

        return true;
      };

      if (_erase()) {
        error_("Invalid value.", __FUNCTION__);
        desync();
      }

    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::vec_erase_by_value_sorted(
      intval value) noexcept {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {

      const auto i = lower_bound(
          m_vec.begin(), m_vec.end(), value,
          [](const vec_row &a, const intval b) -> bool {
            return a.value < b;
          });

      if (i != m_vec.end() && i->value == value) {
        m_vec.erase(i);
      } else {
        error_("Invalid value.", __FUNCTION__);
        desync();
      }

    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::adjust() {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      sets_validate_all();
      bytes_validate_all();
      vec_validate_all();

    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::tick(access::world w) { }

  auto basic_entity::clock() -> bool {

    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      bool const is_tick = m_clock == 0;
      auto const period  = m_sets[n_tick_period].value;

      if (is_tick) {
        m_clock = period - 1;
      } else if (period > 0) {
        m_clock--;
      }

      return is_tick;

    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }

    return false;
  }

  auto basic_entity::is_dynamic() -> bool {

    if (auto _sl = shared_lock(m_lock, lock_timeout); _sl) {
      return m_sets[n_is_dynamic].value > 0;
    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }

    return false;
  }

  auto basic_entity::get_tick_period() -> sl::time {

    if (auto _sl = shared_lock(m_lock, lock_timeout); _sl) {
      return m_sets[n_tick_period].value;
    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }

    return false;
  }

  auto basic_entity::get_id() const -> sl::index {
    return m_id;
  }

  void basic_entity::setup_sets(vsets_row const &sets) {
    m_sets.insert(m_sets.end(), sets.begin(), sets.end());

    sort(m_sets.begin(), m_sets.end(),
         [](sets_row const &a, sets_row const &b) -> bool {
           return a.id < b.id;
         });

    for (sl::index i = 1; i < m_sets.size();) {
      if (m_sets[i - 1].id == m_sets[i].id)
        m_sets.erase(m_sets.begin() + i);
      else
        i++;
    }
  }

  void basic_entity::init(sl::index n, intval value) {
    if (n < 0 || n >= m_sets.size()) {
      error_("Invalid index.", __FUNCTION__);
      desync();
      return;
    }

    m_sets[n].value = value;
    m_sets[n].delta = 0;
  }

  void basic_entity::bytes_init(sl::index n, int8_t value) noexcept {
    if (n < 0 || n >= m_bytes.size()) {
      error_("Invalid index.", __FUNCTION__);
      desync();
      return;
    }

    m_bytes[n].value = value;
    m_bytes[n].delta = 0;
  }

  void basic_entity::vec_init(sl::index n, intval value) noexcept {
    if (n >= 0 && n < m_vec.size()) {
      m_vec[n].value = value;
      m_vec[n].delta = 0;
    }
  }

  void basic_entity::self_destruct(access::world const &w) {
    w.queue(make_unique<action::remove>(this->get_id()));
  }

  void basic_entity::desync() {
    if (auto w = m_world.lock(); w)
      w->desync();
  }

  void basic_entity::assign(cref_entity en) noexcept {
    m_sets_changed  = en.m_sets_changed;
    m_bytes_changed = en.m_bytes_changed;
    m_vec_changed   = en.m_vec_changed;
    m_id            = en.m_id;

    m_sets  = en.m_sets;
    m_bytes = en.m_bytes;
    m_vec   = en.m_vec;
    m_clock = en.m_clock;
    m_world = en.m_world;
  }

  void basic_entity::assign(basic_entity &&en) noexcept {
    m_sets_changed  = move(en.m_sets_changed);
    m_bytes_changed = move(en.m_bytes_changed);
    m_vec_changed   = move(en.m_vec_changed);
    m_id            = move(en.m_id);

    m_sets  = move(en.m_sets);
    m_bytes = move(en.m_bytes);
    m_vec   = move(en.m_vec);
    m_clock = move(en.m_clock);
    m_world = move(en.m_world);
  }

  void basic_entity::sets_invalidate(sl::index const n) noexcept {
    invalidate_range(m_sets_changed, n, 1);
  }

  void basic_entity::sets_validate_all() noexcept {
    if (m_sets_changed.empty())
      return;

    const auto is_dynamic_old = m_sets[n_is_dynamic].value > 0;

    for (auto const &range : m_sets_changed)
      for (sl::index i = range.begin; i != range.end; i++) {
        m_sets[i].value += m_sets[i].delta;
        m_sets[i].delta = 0;
      }

    /*  Respawn if dynamic status changed.
     */

    if (is_dynamic_old != (m_sets[n_is_dynamic].value > 0))
      if (auto w = m_world.lock(); w) {
        w->respawn(m_id);
      }

    m_sets_changed.clear();
  }

  void basic_entity::bytes_invalidate(
      sl::index const n, sl::whole const count) noexcept {
    invalidate_range(m_bytes_changed, n, count);
  }

  void basic_entity::bytes_validate_all() noexcept {
    for (auto const &range : m_bytes_changed)
      for (sl::index i = range.begin; i != range.end; i++) {
        m_bytes[i].value += m_bytes[i].delta;
        m_bytes[i].delta = 0;
      }

    m_bytes_changed.clear();
  }

  void basic_entity::vec_invalidate(sl::index const n,
                                    sl::whole const count) noexcept {
    invalidate_range(m_vec_changed, n, count);
  }

  void basic_entity::vec_validate_all() noexcept {
    for (auto const &range : m_vec_changed)
      for (sl::index i = range.begin; i != range.end; i++) {
        m_vec[i].value += m_vec[i].delta;
        m_vec[i].delta = 0;
      }

    m_vec_changed.clear();
  }

  void basic_entity::invalidate_range(
      sl::vector<_range> &ranges,
      sl::index const     n,
      sl::whole const     count) noexcept {
    if constexpr (!_unsafe) {
      if (count < 0) {
        error_("Invalid count.", __FUNCTION__);
        desync();
        return;
      }
    }

    auto const op = [](_range const &r, sl::index const n) {
      return r.end < n;
    };

    auto const i = lower_bound(ranges.begin(), ranges.end(), n, op);

    if (i == ranges.end()) {
      ranges.emplace_back(n, n + count);
      return;
    }

    if (i->begin > n + count) {
      ranges.emplace(i, n, n + count);
      return;
    }

    if (i->begin > n)
      i->begin = n;

    auto const j = lower_bound(i + 1, ranges.end(), n + count, op);

    if (j == ranges.end() || j->begin > n + count) {
      if (i->end < n + count)
        i->end = n + count;
      ranges.erase(i + 1, j);
    } else {
      i->end = j->end;
      ranges.erase(i + 1, j + 1);
    }
  }
}
