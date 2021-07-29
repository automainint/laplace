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

#ifndef laplace_engine_basic_entity_h
#define laplace_engine_basic_entity_h

#include "access/world.predef.h"
#include "basic_entity.predef.h"
#include "eval/shape.h"
#include "object/sets.h"
#include "world.predef.h"
#include <atomic>
#include <chrono>
#include <memory>
#include <shared_mutex>

namespace laplace::engine {
  class basic_entity {
  public:
    class dummy_tag { };
    class proto_tag { };

    static constexpr auto dummy = dummy_tag {};
    static constexpr auto proto = proto_tag {};

    static const std::chrono::milliseconds lock_timeout;
    static const uint64_t                  default_tick_period;

    basic_entity(cref_entity en) noexcept;
    basic_entity(basic_entity &&en) noexcept;
    auto operator=(cref_entity en) noexcept -> ref_entity;
    auto operator=(basic_entity &&en) noexcept -> ref_entity;

    /*  Prototype initialization.
     */
    basic_entity(proto_tag = proto);

    /*  Dummy initialization.
     */
    basic_entity(dummy_tag);

    /*  Initialize dynamic entity.
     */
    basic_entity(uint64_t tick_period);

    virtual ~basic_entity() = default;

    /*  Set the Entity dynamic status.
     *  Thread-safe.
     */
    void set_dynamic(bool is_dynamic);

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

    /*  Set the Entity id. Do not modify the
     *  id set by the World.
     */
    void set_id(sl::index id);

    void set_world(ptr_world w);
    void reset_world();

    [[nodiscard]] auto index_of(sl::index id) const -> sl::index;
    [[nodiscard]] auto get_count() const -> sl::whole;

    [[nodiscard]] auto id_of(sl::index index) const -> sl::index;
    [[nodiscard]] auto scale_of(sl::index index) const -> sl::index;

    /*  Get a state value.
     *  Thread-safe.
     */
    [[nodiscard]] auto get(sl::index n) -> intval;

    /*  Change a state value. It will apply
     *  calculated delta for the value. The actual
     *  value will change due adjusting.
     *  Thread-safe.
     */
    void set(sl::index n, intval value);

    /*  Apply a state value delta.
     *  Thread-safe.
     */
    void apply_delta(sl::index n, intval delta);

    [[nodiscard]] auto bytes_get_size() noexcept -> sl::whole;

    [[nodiscard]] auto bytes_get(sl::index n) noexcept -> int8_t;

    void bytes_read(sl::index n, std::span<int8_t> dst) noexcept;

    void bytes_set(sl::index n, int8_t value) noexcept;

    void bytes_write(sl::index               n,
                     std::span<const int8_t> values) noexcept;

    void bytes_apply_delta(sl::index n, int8_t delta) noexcept;

    void bytes_write_delta(sl::index               n,
                           std::span<const int8_t> deltas) noexcept;

    void bytes_erase_delta(sl::index               n,
                           std::span<const int8_t> deltas) noexcept;

    void bytes_resize(sl::whole size) noexcept;

    [[nodiscard]] auto vec_get_size() -> sl::whole;

    [[nodiscard]] auto vec_get(sl::index n) -> intval;

    void vec_set(sl::index n, intval value);

    void vec_apply_delta(sl::index n, intval delta);

    void vec_resize(sl::whole size);
    void vec_add(intval value);
    void vec_add_sorted(intval value);
    void vec_insert(sl::index n, intval value);
    void vec_erase(sl::index n);
    void vec_erase_by_value(intval value);
    void vec_erase_by_value_sorted(intval value);

    /*  Adjust Entity new state.
     *  Thread-safe.
     */
    void adjust();

    /*  Dynamic Entity live loop.
     */
    virtual void tick(access::world w);

    /*  Decrement the clock timer. Returns true
     *  if the tick period has expired.
     *
     *  Thread-safe.
     */
    auto clock() -> bool;

    /*  Returns true if the Entity is dynamic.
     *  Thread-safe.
     */
    [[nodiscard]] auto is_dynamic() -> bool;

    /*  Returns the tick period.
     *  Thread-safe.
     */
    [[nodiscard]] auto get_tick_period() -> uint64_t;

    /*  Returns the Entity id.
     */
    [[nodiscard]] auto get_id() const -> sl::index;

    /*  Get a state value by id.
     *  Thread-safe.
     */
    [[nodiscard]] auto get_by_id(sl::index id) -> intval;

  protected:
    /*  State values' indices in PRECISE order.
     */
    enum indices : sl::index {
      n_is_dynamic = 0,
      n_tick_period,
    };

    struct sets_row {
      sl::index id    = id_undefined;
      intval    scale = {};
      intval    value = {};
      intval    delta = {};
    };

    struct bytes_row {
      int8_t value = {};
      int8_t delta = {};
    };

    struct vec_row {
      intval value = {};
      intval delta = {};
    };

    using vsets_row = std::vector<sets_row>;

    /*  Setup state values.
     */
    void setup_sets(const vsets_row &sets);

    /*  Init state value.
     */
    void init(sl::index n, intval value);

    void bytes_init(sl::index n, int8_t value) noexcept;

    void vec_init(sl::index n, intval value);

    /*  Get a state value without
     *  locking.
     */
    [[nodiscard]] auto locked_get(sl::index n) const -> intval;

    /*  Get a state value by id
     *  without locking.
     */
    [[nodiscard]] auto locked_get_by_id(sl::index id) const -> intval;

    [[nodiscard]] auto locked_bytes_get_size() const noexcept
        -> sl::whole;

    [[nodiscard]] auto locked_bytes_get(sl::index n) const noexcept
        -> int8_t;

    [[nodiscard]] auto locked_vec_get_size() const -> sl::whole;

    [[nodiscard]] auto locked_vec_get(sl::index n) const -> intval;

    void self_destruct(const access::world &w);
    void desync();

  private:
    void assign(cref_entity en) noexcept;
    void assign(basic_entity &&en) noexcept;

    std::shared_timed_mutex m_lock;
    std::weak_ptr<world>    m_world;
    vsets_row               m_sets;
    sl::vector<bytes_row>   m_bytes;
    sl::vector<vec_row>     m_vec;
    uint64_t                m_clock            = {};
    sl::index               m_id               = id_undefined;
    bool                    m_is_changed       = false;
    bool                    m_is_bytes_changed = false;
    bool                    m_is_vec_changed   = false;
  };
}

#include "basic_entity.impl.h"

#endif
