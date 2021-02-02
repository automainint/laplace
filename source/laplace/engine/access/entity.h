#ifndef __laplace__engine_access_entity__
#define __laplace__engine_access_entity__

#include "../basic_entity.h"
#include "mode.h"

namespace laplace::engine::access {
  class entity {
  public:
    entity(const entity &) = delete;
    entity &operator=(const entity &) = delete;

    [[nodiscard]] entity(entity &&) noexcept = default;
    [[nodiscard]] entity(ptr_entity ent, mode access_mode);

    entity &operator=(entity &&) = default;

    ~entity() = default;

    [[nodiscard]] auto exist() const -> bool;

    void set_dynamic(bool is_dynamic);
    void set_markable(bool is_markable);
    void set_selectable(bool is_selectable);
    void set_vulnerable(bool is_vulnerable);
    void set_tick_period(size_t tick_period);
    void set_bounds(cref_box val);

    [[nodiscard]] auto index_of(size_t id) -> size_t;
    [[nodiscard]] auto get_count() -> size_t;

    [[nodiscard]] auto id_of(size_t index) -> size_t;
    [[nodiscard]] auto scale_of(size_t index) -> size_t;
    [[nodiscard]] auto get(size_t index) -> int64_t;
    void               set(size_t index, int64_t value);
    void               apply_delta(size_t index, int64_t delta);

    [[nodiscard]] auto request(size_t id, cref_vbyte args = {})
        -> vbyte;

    void modify(size_t id, cref_vbyte args = {});

    [[nodiscard]] auto is_changed() -> bool;
    [[nodiscard]] auto is_dynamic() -> bool;
    [[nodiscard]] auto is_markable() -> bool;
    [[nodiscard]] auto is_selectable() -> bool;
    [[nodiscard]] auto is_vulnerable() -> bool;

    [[nodiscard]] auto get_tick_period() -> size_t;
    [[nodiscard]] auto get_bounds() -> box;
    [[nodiscard]] auto get_id() -> size_t;

  private:
    ptr_entity m_entity;
    mode       m_mode = forbidden;
  };
}

#endif
