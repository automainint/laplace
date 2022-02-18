/*  Copyright (c) 2022 Mitya Selivanov
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
#include "eval/arithmetic.impl.h"
#include "world.h"
#include <algorithm>
#include <chrono>

namespace laplace::engine {
  namespace sets = object::sets;

  using std::unique_lock, std::shared_lock, std::span, std::min,
      std::max, std::move, std::make_unique, std::lower_bound,
      eval::impl::wrap_add, eval::impl::wrap_sub;

  using ms = std::chrono::milliseconds;

  sl::time const basic_entity::lock_timeout_msec   = 300;
  sl::time const basic_entity::default_tick_period = 10;

  basic_entity::basic_entity(basic_entity const &en) noexcept {
    assign(en);
  }

  basic_entity::basic_entity(basic_entity &&en) noexcept {
    assign(std::move(en));
  }

  auto basic_entity::operator=(basic_entity const &en) noexcept
      -> basic_entity & {

    assign(en);
    return *this;
  }

  auto basic_entity::operator=(basic_entity &&en) noexcept
      -> basic_entity & {

    assign(std::move(en));
    return *this;
  }

  basic_entity::basic_entity(proto_tag) noexcept {
    setup_sets({ { sets::is_dynamic, 1, 0 },
                 { sets::tick_period, sets::scale_time,
                   default_tick_period } });
  }

  basic_entity::basic_entity(dummy_tag) noexcept { }

  basic_entity::basic_entity(dynamic_tag,
                             sl::time tick_period) noexcept {
    setup_sets({ { sets::is_dynamic, 1, 1 },
                 { sets::tick_period, sets::scale_time,
                   static_cast<intval>(tick_period) } });
  }

  void basic_entity::set_dynamic(bool is_dynamic) noexcept {
    if (auto _ul = unique_lock(m_lock, ms(lock_timeout_msec)); _ul) {
      m_sets[n_is_dynamic].delta += is_dynamic ? 1 : -1;
      sets_invalidate(n_is_dynamic);
    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::set_tick_period(sl::time tick_period) noexcept {
    if (auto _ul = unique_lock(m_lock, ms(lock_timeout_msec)); _ul) {
      m_sets[n_tick_period].delta += static_cast<int64_t>(
                                         tick_period) -
                                     m_sets[n_tick_period].value;

      sets_invalidate(n_tick_period);
    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::set_clock(sl::time clock_msec) noexcept {
    if (auto _ul = unique_lock(m_lock, ms(lock_timeout_msec)); _ul) {
      m_clock = clock_msec;
    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::reset_clock() noexcept {
    if (auto _ul = unique_lock(m_lock, ms(lock_timeout_msec)); _ul) {
      const auto period = m_sets[n_tick_period].value;
      m_clock           = period - 1;
    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::set_world(ptr_world const &w,
                               sl::index        id) noexcept {
    m_world = w;
    m_id    = id;
  }

  void basic_entity::reset_world() noexcept {
    m_world.reset();
    m_id = id_undefined;
  }

  auto basic_entity::index_of(sl::index id) noexcept -> sl::index {
    if (auto const it = lower_bound(
            m_sets.begin(), m_sets.end(), id,
            [](sets_row const &row, sl::index const id) {
              return row.id < id;
            });
        it != m_sets.end() && it->id == id) {
      return as_index(it - m_sets.begin());
    }

    log(log_event::error, "Invalid id.", __FUNCTION__);
    desync();
    return id_undefined;
  }

  auto basic_entity::get_count() const noexcept -> sl::whole {
    return m_sets.size();
  }

  auto basic_entity::id_of(sl::index n) noexcept -> sl::index {
    if (n < 0 || n >= m_sets.size()) {
      log(log_event::error, "Invalid index.", __FUNCTION__);
      desync();
      return id_undefined;
    }

    return m_sets[n].id;
  }

  auto basic_entity::scale_of(sl::index n) noexcept -> sl::index {
    if (n < 0 || n >= m_sets.size()) {
      log(log_event::error, "Invalid index.", __FUNCTION__);
      desync();
      return 0;
    }

    return m_sets[n].scale;
  }

  auto basic_entity::get(sl::index n) noexcept -> intval {
    if (auto _sl = shared_lock(m_lock, ms(lock_timeout_msec)); _sl) {
      if (n < 0 || n >= m_sets.size()) {
        log(log_event::error, "Invalid index.", __FUNCTION__);
        desync();
        return {};
      }

      return m_sets[n].value;
    }

    log(log_event::error, "Lock timeout.", __FUNCTION__);
    desync();
    return {};
  }

  void basic_entity::set(sl::index n, intval value) noexcept {
    if (auto _ul = unique_lock(m_lock, ms(lock_timeout_msec)); _ul) {
      if (n < 0 || n >= m_sets.size()) {
        log(log_event::error, "Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      m_sets[n].delta = wrap_add(m_sets[n].delta,
                                 wrap_sub(value, m_sets[n].value));
      sets_invalidate(n);

    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::apply_delta(sl::index n, intval delta) noexcept {
    if (auto _ul = unique_lock(m_lock, ms(lock_timeout_msec)); _ul) {
      if (n < 0 || n >= m_sets.size()) {
        log(log_event::error, "Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      m_sets[n].delta = wrap_add(m_sets[n].delta, delta);
      sets_invalidate(n);

    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  auto basic_entity::bytes_get_size() noexcept -> sl::whole {
    if (auto _sl = shared_lock(m_lock, ms(lock_timeout_msec)); _sl) {
      return m_bytes.size();
    }

    log(log_event::error, "Lock timeout.", __FUNCTION__);
    desync();
    return {};
  }

  auto basic_entity::bytes_get(sl::index n) noexcept -> int8_t {
    if (auto _sl = shared_lock(m_lock, ms(lock_timeout_msec)); _sl) {
      if (n < 0 || n >= m_bytes.size()) {
        log(log_event::error, "Invalid index.", __FUNCTION__);
        desync();
        return {};
      }

      return m_bytes[n].value;
    }

    log(log_event::error, "Lock timeout.", __FUNCTION__);
    desync();
    return {};
  }

  auto basic_entity::bytes_get_all() noexcept -> sl::vector<int8_t> {
    if (auto _sl = shared_lock(m_lock, ms(lock_timeout_msec)); _sl) {
      auto v = sl::vector<int8_t> {};
      v.reserve(m_bytes.size());

      for (auto &x : m_bytes) v.emplace_back(x.value);

      return v;
    }

    log(log_event::error, "Lock timeout.", __FUNCTION__);
    desync();
    return {};
  }

  void basic_entity::bytes_read(sl::index    n,
                                span<int8_t> dst) noexcept {
    if (auto _sl = shared_lock(m_lock, ms(lock_timeout_msec)); _sl) {
      if (n + dst.size() > m_bytes.size()) {
        log(log_event::error, "Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      for (sl::index i = 0; i < dst.size(); i++)
        dst.begin()[i] = m_bytes[n + i].value;

    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::bytes_set(sl::index n, int8_t value) noexcept {
    if (auto _ul = unique_lock(m_lock, ms(lock_timeout_msec)); _ul) {
      if (n < 0 || n >= m_bytes.size()) {
        log(log_event::error, "Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      m_bytes[n].delta = wrap_add(m_bytes[n].delta,
                                  wrap_sub(value, m_bytes[n].value));
      bytes_invalidate(n);

    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::bytes_write(sl::index          n,
                                 span<int8_t const> values) noexcept {
    if (auto _ul = unique_lock(m_lock, ms(lock_timeout_msec)); _ul) {
      if (n < 0 || n + values.size() > m_bytes.size()) {
        log(log_event::error, "Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      for (sl::index i = 0; i < values.size(); i++)
        m_bytes[n + i].delta = wrap_add(
            m_bytes[n + i].delta,
            wrap_sub(values[i], m_bytes[n + i].value));

      bytes_invalidate(n, values.size());

    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::bytes_apply_delta(sl::index n,
                                       int8_t    delta) noexcept {
    if (auto _ul = unique_lock(m_lock, ms(lock_timeout_msec)); _ul) {
      if (n < 0 || n >= m_bytes.size()) {
        log(log_event::error, "Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      m_bytes[n].delta = wrap_add(m_bytes[n].delta, delta);
      bytes_invalidate(n);

    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::bytes_write_delta(
      sl::index n, span<int8_t const> deltas) noexcept {
    if (auto _ul = unique_lock(m_lock, ms(lock_timeout_msec)); _ul) {
      if (n < 0 || n + deltas.size() > m_bytes.size()) {
        log(log_event::error, "Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      for (sl::index i = 0; i < deltas.size(); i++)
        m_bytes[n + i].delta = wrap_add(m_bytes[n + i].delta,
                                        deltas[i]);

      bytes_invalidate(n, deltas.size());

    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::bytes_erase_delta(
      sl::index n, span<int8_t const> deltas) noexcept {
    if (auto _ul = unique_lock(m_lock, ms(lock_timeout_msec)); _ul) {
      if (n < 0 || n + deltas.size() > m_bytes.size()) {
        log(log_event::error, "Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      for (sl::index i = 0; i < deltas.size(); i++)
        m_bytes[n + i].delta = wrap_sub(m_bytes[n + i].delta,
                                        deltas[i]);

      bytes_invalidate(n, deltas.size());

    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::bytes_resize(sl::whole size) noexcept {
    if (auto _ul = unique_lock(m_lock, ms(lock_timeout_msec)); _ul) {
      if (size < 0) {
        log(log_event::error, "Invalid size.", __FUNCTION__);
        desync();
        return;
      }

      m_bytes.resize(size);

    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  auto basic_entity::vec_get_size() noexcept -> sl::whole {
    if (auto _sl = shared_lock(m_lock, ms(lock_timeout_msec)); _sl) {
      return m_vec.size();
    }

    log(log_event::error, "Lock timeout.", __FUNCTION__);
    desync();
    return {};
  }

  auto basic_entity::vec_get(sl::index n) noexcept -> intval {
    if (auto _sl = shared_lock(m_lock, ms(lock_timeout_msec)); _sl) {
      if (n < 0 || n >= m_vec.size()) {
        log(log_event::error, "Invalid index.", __FUNCTION__);
        desync();
        return {};
      }

      return m_vec[n].value;
    }

    log(log_event::error, "Lock timeout.", __FUNCTION__);
    desync();
    return {};
  }

  auto basic_entity::vec_get_all() noexcept -> sl::vector<intval> {
    if (auto _sl = shared_lock(m_lock, ms(lock_timeout_msec)); _sl) {
      auto v = sl::vector<intval> {};
      v.reserve(m_vec.size());

      for (auto &x : m_vec) v.emplace_back(x.value);

      return v;
    }

    log(log_event::error, "Lock timeout.", __FUNCTION__);
    desync();
    return {};
  }

  void basic_entity::vec_set(sl::index n, intval value) noexcept {
    if (auto _ul = unique_lock(m_lock, ms(lock_timeout_msec)); _ul) {
      if (n < 0 || n >= m_vec.size()) {
        log(log_event::error, "Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      m_vec[n].delta = wrap_add(m_vec[n].delta,
                                wrap_sub(value, m_vec[n].value));
      vec_invalidate(n);

    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::vec_apply_delta(sl::index n,
                                     intval    delta) noexcept {
    if (auto _ul = unique_lock(m_lock, ms(lock_timeout_msec)); _ul) {
      if (n < 0 || n >= m_vec.size()) {
        log(log_event::error, "Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      m_vec[n].delta = wrap_add(m_vec[n].delta, delta);
      vec_invalidate(n);

    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::vec_read(sl::index    n,
                              span<intval> dst) noexcept {
    if (auto _sl = shared_lock(m_lock, ms(lock_timeout_msec)); _sl) {
      if (n < 0 || n + dst.size() > m_vec.size()) {
        log(log_event::error, "Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      for (sl::index i = 0; i < dst.size(); i++)
        dst[i] = m_vec[n + i].value;

    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::vec_write(sl::index          n,
                               span<intval const> values) noexcept {
    if (auto _ul = unique_lock(m_lock, ms(lock_timeout_msec)); _ul) {
      if (n < 0 || n + values.size() > m_vec.size()) {
        log(log_event::error, "Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      for (sl::index i = 0; i < values.size(); i++) {
        m_vec[n + i].delta = wrap_add(
            m_vec[n + i].delta,
            wrap_sub(values[i], m_vec[n + i].value));
      }

      vec_invalidate(n, values.size());

    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::vec_write_delta(
      sl::index n, span<intval const> deltas) noexcept {
    if (auto _ul = unique_lock(m_lock, ms(lock_timeout_msec)); _ul) {
      if (n < 0 || n + deltas.size() > m_vec.size()) {
        log(log_event::error, "Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      for (sl::index i = 0; i < deltas.size(); i++)
        m_vec[n + i].delta = wrap_add(m_vec[n + i].delta, deltas[i]);

      vec_invalidate(n, deltas.size());

    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::vec_erase_delta(
      sl::index n, span<intval const> deltas) noexcept {
    if (auto _ul = unique_lock(m_lock, ms(lock_timeout_msec)); _ul) {
      if (n < 0 || n + deltas.size() > m_vec.size()) {
        log(log_event::error, "Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      for (sl::index i = 0; i < deltas.size(); i++)
        m_vec[n + i].delta = wrap_sub(m_vec[n + i].delta, deltas[i]);

      vec_invalidate(n, deltas.size());

    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::vec_resize(sl::whole size) noexcept {
    if (auto _ul = unique_lock(m_lock, ms(lock_timeout_msec)); _ul) {
      if (size < 0) {
        log(log_event::error, "Invalid size.", __FUNCTION__);
        desync();
        return;
      }

      m_vec.resize(size);

    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::vec_add(span<intval const> bunch) noexcept {
    if (auto _ul = unique_lock(m_lock, ms(lock_timeout_msec)); _ul) {

      m_vec.reserve(m_vec.size() + bunch.size());
      for (auto row : bunch) m_vec.emplace_back<vec_row>({ row, 0 });

    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::vec_add_sorted(
      span<intval const> bunch) noexcept {
    if (auto _ul = unique_lock(m_lock, ms(lock_timeout_msec)); _ul) {

      if (bunch.empty())
        return;

      vec_do_insert(vec_lower_bound(bunch[0], bunch.size()), bunch);

    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::vec_insert(sl::index          n,
                                span<intval const> bunch) noexcept {
    if (auto _ul = unique_lock(m_lock, ms(lock_timeout_msec)); _ul) {
      if (n < 0 || n > m_vec.size()) {
        log(log_event::error, "Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      vec_do_insert(n, bunch);

    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::vec_erase(sl::index n,
                               sl::whole stride) noexcept {
    if (auto _ul = unique_lock(m_lock, ms(lock_timeout_msec)); _ul) {
      if (n < 0 || stride < 0 || n + stride > m_vec.size()) {
        log(log_event::error, "Invalid index.", __FUNCTION__);
        desync();
        return;
      }

      m_vec.erase(m_vec.begin() + n, m_vec.begin() + n + stride);

    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::vec_erase_by_value(intval    value,
                                        sl::whole stride) noexcept {
    if (auto _ul = unique_lock(m_lock, ms(lock_timeout_msec)); _ul) {

      auto const _erase = [&]() {
        for (sl::index i = 0; i + stride <= m_vec.size(); i += stride)
          if (m_vec[i].value == value) {
            m_vec.erase(m_vec.begin() + i,
                        m_vec.begin() + i + stride);
            return true;
          }

        return false;
      };

      if (!_erase()) {
        log(log_event::error, "Invalid value.", __FUNCTION__);
        desync();
      }

    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::vec_erase_by_value_sorted(
      intval value, sl::whole stride) noexcept {
    if (auto _ul = unique_lock(m_lock, ms(lock_timeout_msec)); _ul) {

      const auto i = vec_lower_bound(value, stride);

      if (i >= 0 && i + stride <= m_vec.size() &&
          m_vec[i].value == value) {
        m_vec.erase(m_vec.begin() + i, m_vec.begin() + i + stride);

      } else {
        log(log_event::error, "Invalid value.", __FUNCTION__);
        desync();
      }

    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::adjust() noexcept {
    if (auto _ul = unique_lock(m_lock, ms(lock_timeout_msec)); _ul) {
      sets_validate_all();
      bytes_validate_all();
      vec_validate_all();

    } else {
      log(log_event::error, "Lock timeout.", __FUNCTION__);
      desync();
    }
  }

  void basic_entity::tick(access::world w) noexcept { }

  auto basic_entity::clock() noexcept -> bool {

    if (auto _ul = unique_lock(m_lock, ms(lock_timeout_msec)); _ul) {
      bool const is_tick = m_clock == 0;
      auto const period  = m_sets[n_tick_period].value;

      if (is_tick) {
        m_clock = period - 1;
      } else if (period > 0) {
        m_clock--;
      }

      return is_tick;
    }

    log(log_event::error, "Lock timeout.", __FUNCTION__);
    desync();
    return false;
  }

  auto basic_entity::is_dynamic() noexcept -> bool {

    if (auto _sl = shared_lock(m_lock, ms(lock_timeout_msec)); _sl) {
      return m_sets[n_is_dynamic].value > 0;
    }

    log(log_event::error, "Lock timeout.", __FUNCTION__);
    desync();
    return false;
  }

  auto basic_entity::get_tick_period() noexcept -> sl::time {
    if (auto _sl = shared_lock(m_lock, ms(lock_timeout_msec)); _sl)
      return m_sets[n_tick_period].value;

    log(log_event::error, "Lock timeout.", __FUNCTION__);
    desync();
    return {};
  }

  auto basic_entity::get_id() const noexcept -> sl::index {
    return m_id;
  }

  void basic_entity::setup_sets(vsets_row const &sets) noexcept {
    m_sets.insert(m_sets.end(), sets.begin(), sets.end());

    sort(m_sets.begin(), m_sets.end(),
         [](sets_row const &a, sets_row const &b) noexcept -> bool {
           return a.id < b.id;
         });

    for (sl::index i = 1; i < m_sets.size();) {
      if (m_sets[i - 1].id == m_sets[i].id) {
        log(log_event::error, __FUNCTION__, "State value duplicate.");
        m_sets.erase(m_sets.begin() + i);
      } else {
        i++;
      }
    }
  }

  void basic_entity::init(sl::index n, intval value) noexcept {
    if (n < 0 || n >= m_sets.size()) {
      log(log_event::error, "Invalid index.", __FUNCTION__);
      return;
    }

    m_sets[n].value = value;
    m_sets[n].delta = 0;
  }

  void basic_entity::bytes_init(sl::index n, int8_t value) noexcept {
    if (n < 0 || n >= m_bytes.size()) {
      log(log_event::error, "Invalid index.", __FUNCTION__);
      return;
    }

    m_bytes[n].value = value;
    m_bytes[n].delta = 0;
  }

  void basic_entity::vec_init(sl::index n, intval value) noexcept {
    if (n < 0 || n >= m_vec.size()) {
      log(log_event::error, "Invalid index.", __FUNCTION__);
      return;
    }

    m_vec[n].value = value;
    m_vec[n].delta = 0;
  }

  void basic_entity::self_destruct(
      access::world const &w) const noexcept {
    w.queue(make_unique<action::remove>(this->get_id()));
  }

  void basic_entity::desync() noexcept {
    if (auto w = m_world.lock(); w)
      w->desync();
  }

  auto basic_entity::_mutex() noexcept -> std::shared_timed_mutex & {
    return m_lock;
  }

  void basic_entity::assign(basic_entity const &en) noexcept {
    if (this == &en)
      return;

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
    m_id            = en.m_id;

    m_sets  = move(en.m_sets);
    m_bytes = move(en.m_bytes);
    m_vec   = move(en.m_vec);
    m_clock = en.m_clock;
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
        m_sets[i].value = wrap_add(m_sets[i].value, m_sets[i].delta);
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
        m_bytes[i].value = wrap_add(m_bytes[i].value,
                                    m_bytes[i].delta);
        m_bytes[i].delta = 0;
      }

    m_bytes_changed.clear();
  }

  void basic_entity::vec_do_insert(
      sl::index n, span<intval const> bunch) noexcept {

    auto v = sl::vector<vec_row> {};
    v.reserve(bunch.size());
    for (auto x : bunch) v.emplace_back<vec_row>({ x, 0 });

    m_vec.insert(m_vec.begin() + n, v.begin(), v.end());
  }

  void basic_entity::vec_invalidate(sl::index const n,
                                    sl::whole const count) noexcept {
    invalidate_range(m_vec_changed, n, count);
  }

  void basic_entity::vec_validate_all() noexcept {
    for (auto const &range : m_vec_changed)
      for (sl::index i = range.begin; i != range.end; i++) {
        m_vec[i].value = wrap_add(m_vec[i].value, m_vec[i].delta);
        m_vec[i].delta = 0;
      }

    m_vec_changed.clear();
  }

  auto basic_entity::vec_lower_bound(intval    value,
                                     sl::whole stride) const noexcept
      -> sl::index {

    if (stride <= 0)
      return 0;

    auto i     = sl::index {};
    auto count = static_cast<sl::index>(m_vec.size()) / stride;

    while (count > 0) {
      auto const half = count / 2;

      i += half;

      if (m_vec[i * stride].value < value) {
        ++i;
        count -= half + 1;
      } else {
        count = half;
      }
    }

    return i * stride;
  }

  void basic_entity::invalidate_range(
      sl::vector<_range> &ranges, sl::index const n,
      sl::whole const count) noexcept {
    if constexpr (!_unsafe) {
      if (count < 0) {
        log(log_event::error, "Invalid count.", __FUNCTION__);
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
