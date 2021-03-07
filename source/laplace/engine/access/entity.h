/*  laplace/engine/access/entity.h
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

#ifndef laplace_engine_access_entity_h
#define laplace_engine_access_entity_h

#include "../basic_entity.h"
#include "mode.h"

namespace laplace::engine::access {
  class entity {
  public:
    entity(const entity &) = delete;
    auto operator=(const entity &) -> entity & = delete;

    [[nodiscard]] entity(entity &&ent) noexcept;
    [[nodiscard]] entity(ptr_entity ent, mode access_mode = read_only);

    auto operator=(entity &&ent) noexcept -> entity &;

    ~entity() = default;

    [[nodiscard]] auto exist() const -> bool;

    void set_dynamic(bool is_dynamic) const;
    void set_markable(bool is_markable) const;
    void set_selectable(bool is_selectable) const;
    void set_vulnerable(bool is_vulnerable) const;
    void set_tick_period(size_t tick_period) const;
    void set_bounds(eval::cref_box val) const;

    [[nodiscard]] auto index_of(size_t id) const -> size_t;
    [[nodiscard]] auto get_count() const -> size_t;

    [[nodiscard]] auto id_of(size_t index) const -> size_t;
    [[nodiscard]] auto scale_of(size_t index) const -> size_t;
    [[nodiscard]] auto get(size_t index) const -> int64_t;

    void set(size_t index, int64_t value) const;
    void apply_delta(size_t index, int64_t delta) const;

    [[nodiscard]] auto request(size_t id, cref_vbyte args = {}) const
        -> vbyte;

    void modify(size_t id, cref_vbyte args = {}) const;

    [[nodiscard]] auto is_changed() const -> bool;
    [[nodiscard]] auto is_dynamic() const -> bool;
    [[nodiscard]] auto is_markable() const -> bool;
    [[nodiscard]] auto is_selectable() const -> bool;
    [[nodiscard]] auto is_vulnerable() const -> bool;

    [[nodiscard]] auto get_tick_period() const -> size_t;
    [[nodiscard]] auto get_bounds() const -> eval::box;
    [[nodiscard]] auto get_id() const -> size_t;

  private:
    ptr_entity m_entity;
    mode       m_mode = forbidden;
  };
}

#endif
