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

namespace laplace::engine {
  namespace sets = object::sets;

  using std::unique_lock, std::shared_lock, std::move, std::make_shared;

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
                   static_cast<int64_t>(tick_period) } });
  }

  void basic_entity::setup_sets(const basic_entity::vsets_row &sets) {

    m_sets.insert(m_sets.end(), sets.begin(), sets.end());

    auto op = [](const sets_row &a, const sets_row &b) -> bool {
      return a.id < b.id;
    };

    sort(m_sets.begin(), m_sets.end(), op);

    for (sl::index i = 1; i < m_sets.size();) {
      if (m_sets[i - 1].id == m_sets[i].id) {
        m_sets.erase(m_sets.begin() + static_cast<ptrdiff_t>(i));
      } else {
        i++;
      }
    }
  }

  void basic_entity::init(sl::index n, intval value) {
    if (n < m_sets.size()) {
      m_sets[n].value = value;
      m_sets[n].delta = 0;
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
    sl::index index = m_sets.size();

    auto op = [](const sets_row &row, sl::index id) {
      return row.id < id;
    };

    auto it = lower_bound(m_sets.begin(), m_sets.end(), id, op);

    if (it != m_sets.end() && it->id == id) {
      index = as_index(it - m_sets.begin());
    }

    return index;
  }

  auto basic_entity::get_count() const -> sl::whole {
    return m_sets.size();
  }

  auto basic_entity::id_of(sl::index index) const -> sl::index {
    return index < m_sets.size() ? m_sets[index].id : id_undefined;
  }

  auto basic_entity::scale_of(sl::index index) const -> sl::index {
    return index < m_sets.size() ? m_sets[index].scale : 0;
  }

  auto basic_entity::get(sl::index index) -> intval {
    if (auto _sl = shared_lock(m_lock, lock_timeout); _sl) {
      return locked_get(index);
    }

    error_("Lock timeout.", __FUNCTION__);
    desync();
    return 0;
  }

  void basic_entity::set(sl::index index, intval value) {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      if (index < m_sets.size()) {
        m_sets[index].delta += value - m_sets[index].value;
        m_is_changed = true;
      }
    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::apply_delta(sl::index index, intval delta) {
    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      if (index < m_sets.size()) {
        m_sets[index].delta += delta;
        m_is_changed = true;
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
    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  auto basic_entity::request( //
      sl::index  id,          //
      span_cbyte args) -> vbyte {

    if (auto _sl = shared_lock(m_lock, lock_timeout); _sl) {
      return this->do_request(id, args);
    } else {
      error_("Lock timeout.", __FUNCTION__);
      desync();
    }

    return {};
  }

  void basic_entity::modify( //
      sl::index  id,         //
      span_cbyte args) {

    if (auto _ul = unique_lock(m_lock, lock_timeout); _ul) {
      this->do_modify(id, args);
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

  auto basic_entity::is_changed() -> bool {

    if (auto _sl = shared_lock(m_lock, lock_timeout); _sl) {
      return m_is_changed;
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

  auto basic_entity::locked_get(sl::index index) const -> intval {
    return index < m_sets.size() ? m_sets[index].value : 0;
  }

  void basic_entity::self_destruct(const access::world &w) {
    w.queue(make_shared<action::remove>(this->get_id()));
  }

  void basic_entity::desync() {
    if (auto w = m_world.lock(); w) {
      w->desync();
    }
  }

  auto basic_entity::do_request(sl::index id, span_cbyte args) const
      -> vbyte {
    return {};
  }

  void basic_entity::do_modify(sl::index id, span_cbyte args) { }

  void basic_entity::assign(cref_entity en) noexcept {
    m_is_changed = en.m_is_changed;
    m_id         = en.m_id;

    m_sets  = en.m_sets;
    m_clock = en.m_clock;
    m_world = en.m_world;
  }

  void basic_entity::assign(basic_entity &&en) noexcept {
    m_is_changed = move(en.m_is_changed);
    m_id         = move(en.m_id);

    m_sets  = move(en.m_sets);
    m_clock = move(en.m_clock);
    m_world = move(en.m_world);
  }
}
