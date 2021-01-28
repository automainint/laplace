#pragma once

#include "mode.h"
#include "../entity.h"

namespace laplace::engine::access
{
    class entity
    {
    public:
        entity(const entity &) = delete;
        entity(entity &&);
        entity &operator =(const entity &) = delete;
        entity &operator =(entity &&) = delete;

        entity(ptr_entity ent, mode access_mode);
        ~entity() = default;

        auto exist() const -> bool;

        void set_dynamic(bool is_dynamic);
        void set_markable(bool is_markable);
        void set_selectable(bool is_selectable);
        void set_vulnerable(bool is_vulnerable);
        void set_tick_period(size_t tick_period);
        void set_bounds(cref_box val);

        auto index_of(size_t id) -> size_t;
        auto get_count() -> size_t;

        auto id_of(size_t index) -> size_t;
        auto scale_of(size_t index) -> size_t;
        auto get(size_t index) -> int64_t;
        void set(size_t index, int64_t value);
        void apply_delta(size_t index, int64_t delta);

        auto request(size_t id, cref_vbyte args = { }) -> vbyte;
        void modify(size_t id, cref_vbyte args = { });

        auto is_changed() -> bool;
        auto is_dynamic() -> bool;
        auto is_markable() -> bool;
        auto is_selectable() -> bool;
        auto is_vulnerable() -> bool;

        auto get_tick_period() -> size_t;
        auto get_bounds() -> box;
        auto get_id() -> size_t;

    private:
        ptr_entity  m_entity;
        mode        m_mode      = forbidden;
    };
}
