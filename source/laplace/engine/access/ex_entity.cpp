/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "entity.h"

namespace laplace::engine::access {
  using std::span;

  entity::entity(entity &&ent) noexcept :
      m_entity(std::move(ent.m_entity)),
      m_mode(std::move(ent.m_mode)) {
    ent.m_mode = forbidden;
  }

  entity::entity(ptr_entity ent, mode access_mode) noexcept :
      m_entity(ent), m_mode(ent ? access_mode : forbidden) { }

  auto entity::operator=(entity &&ent) noexcept -> entity & {
    m_entity   = std::move(ent.m_entity);
    m_mode     = std::move(ent.m_mode);
    ent.m_mode = forbidden;
    return *this;
  }

  auto entity::exist() const noexcept -> bool {
    return m_entity ? true : false;
  }

  void entity::set_dynamic(bool is_dynamic) const noexcept {
    if (is_allowed(async, m_mode))
      m_entity->set_dynamic(is_dynamic);
  }

  void entity::set_tick_period(sl::index tick_period) const noexcept {
    if (is_allowed(async, m_mode))
      m_entity->set_tick_period(tick_period);
  }

  auto entity::index_of(sl::index id) const noexcept -> sl::index {
    return is_allowed(read_only, m_mode) ? m_entity->index_of(id)
                                         : id_undefined;
  }

  auto entity::get_count() const noexcept -> sl::whole {
    return is_allowed(read_only, m_mode) ? m_entity->get_count() : 0;
  }

  auto entity::id_of(sl::index n) const noexcept -> sl::index {
    return is_allowed(read_only, m_mode) ? m_entity->id_of(n)
                                         : id_undefined;
  }

  auto entity::scale_of(sl::index n) const noexcept -> sl::index {
    return is_allowed(read_only, m_mode) ? m_entity->scale_of(n) : 0;
  }

  auto entity::get(sl::index n, intval defval) const noexcept
      -> intval {
    return is_allowed(read_only, m_mode) ? m_entity->get(n) : defval;
  }

  void entity::set(sl::index n, intval value) const noexcept {
    if (is_allowed(async, m_mode))
      m_entity->set(n, value);
  }

  void entity::apply_delta(sl::index n, intval delta) const noexcept {
    if (is_allowed(async, m_mode))
      m_entity->apply_delta(n, delta);
  }

  auto entity::bytes_get_size() const noexcept -> sl::whole {
    return is_allowed(read_only, m_mode) ? m_entity->bytes_get_size()
                                         : 0;
  }

  auto entity::bytes_get(sl::index n, int8_t defval) const noexcept
      -> int8_t {
    return is_allowed(read_only, m_mode) ? m_entity->bytes_get(n)
                                         : defval;
  }

  auto entity::bytes_get_all() const noexcept -> sl::vector<int8_t> {
    return is_allowed(read_only, m_mode) ? m_entity->bytes_get_all()
                                         : sl::vector<int8_t> {};
  }

  void entity::bytes_set(sl::index n, int8_t value) const noexcept {
    if (is_allowed(read_only, m_mode))
      m_entity->bytes_set(n, value);
  }

  void entity::bytes_apply_delta(sl::index n,
                                 int8_t    delta) const noexcept {
    if (is_allowed(read_only, m_mode))
      m_entity->bytes_apply_delta(n, delta);
  }

  void entity::bytes_read(sl::index    n,
                          span<int8_t> dst) const noexcept {
    if (is_allowed(read_only, m_mode))
      m_entity->bytes_read(n, dst);
  }

  void entity::bytes_write(sl::index          n,
                           span<int8_t const> values) const noexcept {
    if (is_allowed(async, m_mode))
      m_entity->bytes_write(n, values);
  }

  void entity::bytes_write_delta(
      sl::index n, span<int8_t const> deltas) const noexcept {
    if (is_allowed(async, m_mode))
      m_entity->bytes_write_delta(n, deltas);
  }

  void entity::bytes_erase_delta(
      sl::index n, span<int8_t const> deltas) const noexcept {
    if (is_allowed(async, m_mode))
      m_entity->bytes_erase_delta(n, deltas);
  }

  void entity::bytes_resize(sl::whole size) const noexcept {
    if (is_allowed(sync, m_mode))
      m_entity->bytes_resize(size);
  }

  auto entity::vec_get_size() const noexcept -> sl::whole {
    return is_allowed(read_only, m_mode) ? m_entity->vec_get_size()
                                         : 0;
  }

  auto entity::vec_get(sl::index n, intval defval) const noexcept
      -> intval {
    return is_allowed(read_only, m_mode) ? m_entity->vec_get(n)
                                         : defval;
  }

  auto entity::vec_get_all() const noexcept -> sl::vector<intval> {
    return is_allowed(read_only, m_mode) ? m_entity->vec_get_all()
                                         : sl::vector<intval> {};
  }

  void entity::vec_read(sl::index    n,
                        span<intval> dst) const noexcept {
    if (is_allowed(read_only, m_mode))
      m_entity->vec_read(n, dst);
  }

  void entity::vec_write(sl::index          n,
                         span<intval const> values) const noexcept {
    if (is_allowed(async, m_mode))
      m_entity->vec_write(n, values);
  }

  void entity::vec_write_delta(
      sl::index n, span<intval const> deltas) const noexcept {
    if (is_allowed(async, m_mode))
      m_entity->vec_write_delta(n, deltas);
  }

  void entity::vec_erase_delta(
      sl::index n, span<intval const> deltas) const noexcept {
    if (is_allowed(async, m_mode))
      m_entity->vec_erase_delta(n, deltas);
  }

  void entity::vec_set(sl::index n, intval value) const noexcept {
    if (is_allowed(async, m_mode))
      m_entity->vec_set(n, value);
  }

  void entity::vec_apply_delta(sl::index n,
                               intval    delta) const noexcept {
    if (is_allowed(async, m_mode))
      m_entity->vec_apply_delta(n, delta);
  }

  void entity::vec_resize(sl::whole size) const noexcept {
    if (is_allowed(sync, m_mode))
      m_entity->vec_resize(size);
  }

  void entity::vec_add(intval value) const noexcept {
    if (is_allowed(sync, m_mode))
      m_entity->vec_add({ &value, 1 });
  }

  void entity::vec_add_sorted(intval value) const noexcept {
    if (is_allowed(sync, m_mode))
      m_entity->vec_add_sorted({ &value, 1 });
  }

  void entity::vec_insert(sl::index n, intval value) const noexcept {
    if (is_allowed(sync, m_mode))
      m_entity->vec_insert(n, { &value, 1 });
  }

  void entity::vec_add(span<intval const> bunch) const noexcept {
    if (is_allowed(sync, m_mode))
      m_entity->vec_add(bunch);
  }

  void entity::vec_add_sorted(
      span<intval const> bunch) const noexcept {
    if (is_allowed(sync, m_mode))
      m_entity->vec_add_sorted(bunch);
  }

  void entity::vec_insert(sl::index          n,
                          span<intval const> bunch) const noexcept {
    if (is_allowed(sync, m_mode))
      m_entity->vec_insert(n, bunch);
  }

  void entity::vec_erase(sl::index n,
                         sl::whole stride) const noexcept {
    if (is_allowed(sync, m_mode))
      m_entity->vec_erase(n, stride);
  }

  void entity::vec_erase_by_value(intval    value,
                                  sl::whole stride) const noexcept {
    if (is_allowed(sync, m_mode))
      m_entity->vec_erase_by_value(value, stride);
  }

  void entity::vec_erase_by_value_sorted(
      intval value, sl::whole stride) const noexcept {
    if (is_allowed(sync, m_mode))
      m_entity->vec_erase_by_value_sorted(value, stride);
  }

  void entity::adjust() const noexcept {
    if (is_allowed(sync, m_mode))
      m_entity->adjust();
  }

  auto entity::is_dynamic() const noexcept -> bool {
    return is_allowed(read_only, m_mode) ? m_entity->is_dynamic()
                                         : false;
  }

  auto entity::get_tick_period() const noexcept -> uint64_t {
    return is_allowed(read_only, m_mode) ? m_entity->get_tick_period()
                                         : 0;
  }

  auto entity::get_id() const noexcept -> sl::index {
    return is_allowed(read_only, m_mode) ? m_entity->get_id()
                                         : id_undefined;
  }
}
