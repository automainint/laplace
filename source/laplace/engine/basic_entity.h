/*  laplace/engine/basic_entity.h
 *
 *      Basic class for any gameplay object.
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

#ifndef __laplace__engine_basic_entity__
#define __laplace__engine_basic_entity__

#include "access/world.predef.h"
#include "basic_entity.predef.h"
#include "object/sets.h"
#include "shape.h"
#include "world.predef.h"
#include <memory>
#include <shared_mutex>

namespace laplace::engine {
  /*  TODO
   *  Atomics, constexpr.
   */
  class basic_entity {
  public:
    enum dummy_tag { dummy };
    enum proto_tag { proto };

    static constexpr uint64_t default_tick_period = 10;

    basic_entity(cref_entity en);
    basic_entity(basic_entity &&en) noexcept;
    auto operator=(cref_entity en) -> ref_entity;
    auto operator=(basic_entity &&en) noexcept -> ref_entity;

    /*  Prototype initialization.
     */
    basic_entity(proto_tag = proto);

    /*  Dummy initialization.
     */
    basic_entity(dummy_tag);

    basic_entity(bool is_dynamic, bool is_markable = false,
                 bool     is_selectable = false,
                 bool     is_vulnerable = false,
                 uint64_t tick_period   = default_tick_period,
                 cref_box bounds        = box {});

    virtual ~basic_entity() = default;

    /*  Set the Entity dynamic status.
     *  Thread-safe.
     */
    void set_dynamic(bool is_dynamic);

    /*  Set the Entity markability status.
     *  Thread-safe.
     */
    void set_markable(bool is_markable);

    /*  Set the Entity selectability status.
     *  Thread-safe.
     */
    void set_selectable(bool is_selectable);

    /*  Set the Entity vulnerability status.
     *  Thread-safe.
     */
    void set_vulnerable(bool is_vulnerable);

    /*  Set the Entity tick period.
     *  Thread-safe.
     */
    void set_tick_period(uint64_t tick_period);

    /*  Set the Entity clock current time.
     *  Thread-safe.
     */
    void set_clock(uint64_t clock_msec);

    /*  Set the Entity clock.
     *  Thread-safe.
     */
    void reset_clock();

    /*  Set the Entity bounding box.
     *  Thread-safe.
     */
    void set_bounds(cref_box val);

    /*  Set the Entity id. Do not modify the
     *  id set by the World.
     */
    void set_id(size_t id);

    void set_world(ptr_world w);
    void reset_world();

    [[nodiscard]] auto index_of(size_t id) const -> size_t;
    [[nodiscard]] auto get_count() const -> size_t;

    [[nodiscard]] auto id_of(size_t index) const -> size_t;
    [[nodiscard]] auto scale_of(size_t index) const -> size_t;

    /*  Get a state value.
     *  Thread-safe.
     */
    [[nodiscard]] auto get(size_t index) -> int64_t;

    /*  Change a state value. It will apply
     *  calculated delta for the value. The actual
     *  value will change due adjusting.
     *  Thread-safe.
     */
    void set(size_t index, int64_t value);

    /*  Apply a state value delta.
     *  Thread-safe.
     */
    void apply_delta(size_t index, int64_t delta);

    /*  Adjust Entity new state.
     *  Thread-safe.
     */
    void adjust();

    /*  Perform an universal request.
     *  Thread-safe.
     */
    [[nodiscard]] auto request(size_t id, cref_vbyte args = {})
        -> vbyte;

    /*  Perform an universal modification.
     *  Thread-safe.
     */
    void modify(size_t id, cref_vbyte args = {});

    /*  Dynamic Entity live loop.
     */
    virtual void tick(access::world w);

    /*  Decrement the clock timer. Returns true
     *  if the tick period has expired.
     *
     *  Thread-safe.
     */
    auto clock() -> bool;

    /*  Returns true if the Entity was changed.
     *  Thread-safe.
     */
    [[nodiscard]] auto is_changed() -> bool;

    /*  Returns true if the Entity is dynamic.
     *  Thread-safe.
     */
    [[nodiscard]] auto is_dynamic() -> bool;

    /*  Returns true if the Entity is markable.
     *  Thread-safe.
     */
    [[nodiscard]] auto is_markable() -> bool;

    /*  Returns true if the Entity is selectable.
     *  Thread-safe.
     */
    [[nodiscard]] auto is_selectable() -> bool;

    /*  Returns true if the Entity is vulnerable.
     *  Thread-safe.
     */
    [[nodiscard]] auto is_vulnerable() -> bool;

    /*  Returns the tick period.
     *  Thread-safe.
     */
    [[nodiscard]] auto get_tick_period() -> uint64_t;

    /*  Returns the bounding box.
     *  Thread-safe.
     */
    [[nodiscard]] auto get_bounds() -> box;

    /*  Returns the Entity id.
     */
    [[nodiscard]] auto get_id() const -> size_t;

    /*  Get a state value by id.
     *  Thread-safe.
     */
    [[nodiscard]] auto get_by_id(size_t id) -> int64_t;

  protected:
    /*  State values' indices in PRECISE order.
     */
    enum indices : size_t {
      n_is_dynamic = 0,
      n_is_markable,
      n_is_selectable,
      n_is_vulnerable,
      n_tick_period,
      n_bounds_min_x,
      n_bounds_min_y,
      n_bounds_min_z,
      n_bounds_max_x,
      n_bounds_max_y,
      n_bounds_max_z
    };

    struct sets_row {
      size_t  id    = id_undefined;
      size_t  scale = 0;
      int64_t value = 0;
      int64_t delta = 0;
    };

    using vsets_row      = std::vector<sets_row>;
    using cref_vsets_row = const vsets_row &;

    std::shared_mutex m_lock;

    /*  Setup state values.
     */
    void setup_sets(cref_vsets_row sets);

    /*  Init state value.
     */
    void init(size_t index, int64_t value);

    /*  Get a state value without
     *  locking.
     */
    [[nodiscard]] auto locked_get(size_t index) const -> int64_t;

    /*  Get a state value by id
     *  without locking.
     */
    [[nodiscard]] auto locked_get_by_id(size_t id) const
        -> int64_t;

    void self_destruct(access::world w);

    /*  Universal request implementation.
     *
     *  Thread-safe methods not allowed
     *  from here due to the locking.
     */
    [[nodiscard]] virtual auto do_request(size_t     id,
                                          cref_vbyte args) const
        -> vbyte;

    /*  Universal modification implementation.
     *
     *  Thread-safe methods not allowed
     *  from here due to the locking.
     */
    virtual void do_modify(size_t id, cref_vbyte args);

  private:
    void assign(cref_entity en);
    void assign(basic_entity &&en);

    bool   m_is_changed = false;
    size_t m_id         = id_undefined;

    vsets_row m_sets;
    uint64_t  m_clock = 0;

    std::weak_ptr<world> m_world;
  };
}

#include "basic_entity.impl.h"

#endif
