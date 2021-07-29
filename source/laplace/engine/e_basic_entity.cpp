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

  using std::unique_lock, std::shared_lock, std::span, std::move,
      std::make_shared, std::lower_bound, std::chrono::milliseconds;

  const milliseconds basic_entity::lock_timeout = milliseconds(100);
  const uint64_t     basic_entity::default_tick_period = 10;

  basic_entity::basic_entity(cref_entity en) noexcept {
    assign(en);
  }

  basic_entity::basic_entity(basic_entity &&en) noexcept {
    assign(en);
  }

  auto basic_entity::operator=(cref_entity en) noexcept -> ref_entity {

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

  basic_entity::basic_entity(uint64_t tick_period) {

    setup_sets({ { sets::is_dynamic, 1, 1 },
                 { sets::tick_period, sets::scale_time,
                   static_cast<intval>(tick_period) } });
  }

  void basic_entity::setup_sets(const basic_entity::vsets_row &sets) {

    m_sets.insert(m_sets.end(), sets.begin(), sets.end());

    auto op = [](const sets_row &a, const sets_row &b) -> bool {
      return a.id < b.id;
    };

    sort(m_sets.begin(), m_sets.end(), op);

    for (sl::index i = 1; i < m_sets.size();) {
      if (m_sets[i - 1].id == m_sets[i].id) {
        m_sets.erase(m_sets.begin() + i);
      } else {
        i++;
      }
    }
  }

  void basic_entity::init(sl::index n, intval value) {
    if (n >= 0 && n < m_sets.size()) {
      m_sets[n].value = value;
      m_sets[n].delta = 0;
    }
  }

  void basic_entity::bytes_init(sl::index n, int8_t value) noexcept {
    if (n >= 0 && n < m_bytes.size()) {
      m_bytes[n].value = value;
      m_bytes[n].delta = 0;
    }
  }

  void basic_entity::vec_init(sl::index n, intval value) {
    if (n >= 0 && n < m_vec.size()) {
      m_vec[n].value = value;
      m_vec[n].delta = 0;
    }
  }

  void basic_entity::set_dynamic(bool is_dynamic) {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      m_sets[n_is_dynamic].delta += is_dynamic ? 1 : -1;
      m_is_changed = true;
    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::set_tick_period(uint64_t tick_period) {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      m_sets[n_tick_period].delta += static_cast<int64_t>(tick_period) -
                                     m_sets[n_tick_period].value;

      m_is_changed = true;
    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::set_clock(uint64_t clock_msec) {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      m_clock = clock_msec;
    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::reset_clock() {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      const auto period = static_cast<uint64_t>(
          m_sets[n_tick_period].value);

      m_clock = period - 1;
    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::set_id(sl::index id) {
    m_id = id;
  }

  void basic_entity::set_world(ptr_world w) {
    m_world = w;
  }

  void basic_entity::reset_world() {
    m_world.reset();
  }

  auto basic_entity::index_of(sl::index id) const -> sl::index {
    sl::index n = m_sets.size();

    auto op = [](const sets_row &row, sl::index id) {
      return row.id < id;
    };

    auto it = lower_bound(m_sets.begin(), m_sets.end(), id, op);

    if (it != m_sets.end() && it->id == id) {
      n = as_index(it - m_sets.begin());
    }

    return n;
  }

  auto basic_entity::get_count() const -> sl::whole {
    return m_sets.size();
  }

  auto basic_entity::id_of(sl::index n) const -> sl::index {
    return n >= 0 && n < m_sets.size() ? m_sets[n].id : id_undefined;
  }

  auto basic_entity::scale_of(sl::index n) const -> sl::index {
    return n >= 0 && n < m_sets.size() ? m_sets[n].scale : 0;
  }

  auto basic_entity::get(sl::index n) -> intval {
    if (auto _sl = shared_lock(m_lock, lock_timeout); _sl) {
      return locked_get(n);
    }

    error_("Lock timeout.", __FUNCTION__);
    desync();
    return 0;
  }

  void basic_entity::set(sl::index n, intval value) {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      if (n < m_sets.size()) {
        m_sets[n].delta += value - m_sets[n].value;
        m_is_changed = true;
      }
    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::apply_delta(sl::index n, intval delta) {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      if (n < m_sets.size()) {
        m_sets[n].delta += delta;
        m_is_changed = true;
      }
    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  auto basic_entity::bytes_get_size() noexcept -> sl::whole {
    if (auto _sl = shared_lock(m_lock, lock_timeout); _sl) {
      return locked_bytes_get_size();
    }

    error_("Lock timeout.", __FUNCTION__);
    desync();
    return {};
  }

  auto basic_entity::bytes_get(sl::index n) noexcept -> int8_t {
    if (auto _sl = shared_lock(m_lock, lock_timeout); _sl) {
      return locked_bytes_get(n);
    }

    error_("Lock timeout.", __FUNCTION__);
    desync();
    return {};
  }

  void basic_entity::bytes_read(sl::index n, span<int8_t> dst) noexcept {
    if (auto _sl = shared_lock(m_lock, lock_timeout); _sl) {
      for (sl::index i = 0; i < dst.size(); i++)
        dst.begin()[i] = locked_bytes_get(n + i);
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
      m_is_bytes_changed = true;

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

      for (sl::index i = 0; i < values.size(); i++)
        m_bytes[n + i].delta += values[i] - m_bytes[n + i].value;

      m_is_bytes_changed = true;

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
      m_is_bytes_changed = true;

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

      for (sl::index i = 0; i < deltas.size(); i++)
        m_bytes[n + i].delta += deltas[i];

      m_is_bytes_changed = true;

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

      for (sl::index i = 0; i < deltas.size(); i++)
        m_bytes[n + i].delta -= deltas[i];

      m_is_bytes_changed = true;

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

  auto basic_entity::vec_get_size() -> sl::whole {
    if (auto _sl = shared_lock(m_lock, lock_timeout); _sl) {
      return locked_vec_get_size();
    }

    error_("Lock timeout.", __FUNCTION__);
    desync();
    return {};
  }

  auto basic_entity::vec_get(sl::index n) -> intval {
    if (auto _sl = shared_lock(m_lock, lock_timeout); _sl) {
      return locked_vec_get(n);
    }

    error_("Lock timeout.", __FUNCTION__);
    desync();
    return {};
  }

  void basic_entity::vec_set(sl::index n, intval value) {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      if (n < 0 || n >= m_vec.size()) {
        error_("Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      m_vec[n].delta += value - m_vec[n].value;
      m_is_vec_changed = true;

    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::vec_apply_delta(sl::index n, intval delta) {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      if (n < 0 || n >= m_vec.size()) {
        error_("Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      m_vec[n].delta += delta;
      m_is_vec_changed = true;

    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::vec_resize(sl::whole size) {
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

  void basic_entity::vec_add(intval value) {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {

      m_vec.emplace_back(vec_row { value, 0 });

    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::vec_add_sorted(intval value) {
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

  void basic_entity::vec_insert(sl::index n, intval value) {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      if (n < 0 || n > m_vec.size()) {
        error_("Invalid idnex.", __FUNCTION__);
        desync();
        return;
      }

      m_vec.insert(m_vec.begin() + n, 1, { value, 0 });

    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::vec_erase(sl::index n) {
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

  void basic_entity::vec_erase_by_value(intval value) {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {

      [this, value]() {
        for (auto i = m_vec.begin(); i != m_vec.end(); i++) {
          if (i->value == value) {
            m_vec.erase(i);
            return;
          }
        }
      }();

    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::vec_erase_by_value_sorted(intval value) {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {

      const auto i = lower_bound(
          m_vec.begin(), m_vec.end(), value,
          [](const vec_row &a, const intval b) -> bool {
            return a.value < b;
          });

      if (i != m_vec.end() && i->value == value) {
        m_vec.erase(i);
      }

    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::adjust() {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      if (m_is_changed) {
        const auto is_dynamic_old = m_sets[n_is_dynamic].value > 0;

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

      if (m_is_bytes_changed) {
        for (auto &x : m_bytes) {
          x.value += x.delta;
          x.delta = 0;
        }

        m_is_bytes_changed = false;
      }

      if (m_is_vec_changed) {
        for (auto &x : m_vec) {
          x.value += x.delta;
          x.delta = 0;
        }

        m_is_vec_changed = false;
      }
    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::tick(access::world w) { }

  auto basic_entity::clock() -> bool {

    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      const bool result = m_clock == 0;

      const auto period = static_cast<uint64_t>(
          m_sets[n_tick_period].value);

      if (m_clock == 0) {
        m_clock = period - 1;
      } else if (period > 0) {
        m_clock--;
      }

      return result;

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

  auto basic_entity::get_tick_period() -> uint64_t {

    if (auto _sl = shared_lock(m_lock, lock_timeout); _sl) {
      return static_cast<uint64_t>(m_sets[n_tick_period].value);
    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }

    return false;
  }

  auto basic_entity::get_id() const -> sl::index {
    return m_id;
  }

  auto basic_entity::locked_get(sl::index n) const -> intval {
    return n >= 0 && n < m_sets.size() ? m_sets[n].value : 0;
  }

  auto basic_entity::locked_bytes_get_size() const noexcept
      -> sl::whole {
    return m_bytes.size();
  }

  auto basic_entity::locked_bytes_get(sl::index n) const noexcept
      -> int8_t {
    return n >= 0 && n < m_bytes.size() ? m_bytes[n].value : 0;
  }

  auto basic_entity::locked_vec_get_size() const -> sl::whole {
    return m_vec.size();
  }

  auto basic_entity::locked_vec_get(sl::index n) const -> intval {
    return n >= 0 && n < m_vec.size() ? m_vec[n].value : 0;
  }

  void basic_entity::self_destruct(const access::world &w) {
    w.queue(make_shared<action::remove>(this->get_id()));
  }

  void basic_entity::desync() {
    if (auto w = m_world.lock(); w) {
      w->desync();
    }
  }

  void basic_entity::assign(cref_entity en) noexcept {
    m_is_changed       = en.m_is_changed;
    m_is_bytes_changed = en.m_is_bytes_changed;
    m_is_vec_changed   = en.m_is_vec_changed;
    m_id               = en.m_id;

    m_sets  = en.m_sets;
    m_bytes = en.m_bytes;
    m_vec   = en.m_vec;
    m_clock = en.m_clock;
    m_world = en.m_world;
  }

  void basic_entity::assign(basic_entity &&en) noexcept {
    m_is_changed       = move(en.m_is_changed);
    m_is_bytes_changed = move(en.m_is_bytes_changed);
    m_is_vec_changed   = move(en.m_is_vec_changed);
    m_id               = move(en.m_id);

    m_sets  = move(en.m_sets);
    m_bytes = move(en.m_bytes);
    m_vec   = move(en.m_vec);
    m_clock = move(en.m_clock);
    m_world = move(en.m_world);
  }
}
