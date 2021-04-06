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
    void set_tick_period(size_t tick_period) const;

    [[nodiscard]] auto index_of(size_t id) const -> size_t;
    [[nodiscard]] auto get_count() const -> size_t;

    [[nodiscard]] auto id_of(size_t index) const -> size_t;
    [[nodiscard]] auto scale_of(size_t index) const -> size_t;
    [[nodiscard]] auto get(size_t index, int64_t defval = 0) const
        -> int64_t;

    void set(size_t index, int64_t value) const;
    void apply_delta(size_t index, int64_t delta) const;
    void adjust() const;

    [[nodiscard]] auto request(size_t id, span_cbyte args = {}) const
        -> vbyte;

    void modify(size_t id, span_cbyte args = {}) const;

    [[nodiscard]] auto is_dynamic() const -> bool;
    [[nodiscard]] auto get_tick_period() const -> size_t;
    [[nodiscard]] auto get_id() const -> size_t;

  private:
    ptr_entity m_entity;
    mode       m_mode = forbidden;
  };
}

#endif
