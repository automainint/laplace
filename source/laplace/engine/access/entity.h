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
    void set_tick_period(sl::index tick_period) const;

    [[nodiscard]] auto index_of(sl::index id) const -> sl::index;
    [[nodiscard]] auto get_count() const -> sl::whole;

    [[nodiscard]] auto id_of(sl::index n) const -> sl::index;
    [[nodiscard]] auto scale_of(sl::index n) const -> sl::index;
    [[nodiscard]] auto get(sl::index n, intval defval = {}) const
        -> intval;

    void set(sl::index n, intval value) const;
    void apply_delta(sl::index n, intval delta) const;

    [[nodiscard]] auto bytes_get_size() const noexcept -> sl::whole;

    [[nodiscard]] auto bytes_get(sl::index n,
                                 int8_t    defval = {}) const noexcept
        -> int8_t;

    void bytes_read(sl::index n, std::span<int8_t> dst) const noexcept;

    void bytes_set(sl::index n, int8_t value) const noexcept;
    void bytes_write(sl::index               n,
                     std::span<const int8_t> values) const noexcept;
    void bytes_apply_delta(sl::index n, int8_t delta) const noexcept;
    void bytes_write_delta(sl::index               n,
                           std::span<const int8_t> deltas) const noexcept;
    void bytes_erase_delta(sl::index               n,
                           std::span<const int8_t> deltas) const noexcept;

    void bytes_resize(sl::whole size) const noexcept;

    [[nodiscard]] auto vec_get_size() const -> sl::whole;

    [[nodiscard]] auto vec_get(sl::index n, intval defval = {}) const
        -> intval;

    void vec_set(sl::index n, intval value) const;
    void vec_apply_delta(sl::index n, intval delta) const;
    void vec_resize(sl::whole size) const;
    void vec_add(intval value) const;
    void vec_add_sorted(intval value) const;
    void vec_insert(sl::index n, intval value) const;
    void vec_erase(sl::index n) const;
    void vec_erase_by_value(intval value) const;
    void vec_erase_by_value_sorted(intval value) const;

    void adjust() const;

    [[nodiscard]] auto is_dynamic() const -> bool;
    [[nodiscard]] auto get_tick_period() const -> uint64_t;
    [[nodiscard]] auto get_id() const -> sl::index;

  private:
    ptr_entity m_entity;
    mode       m_mode = forbidden;
  };
}

#endif
