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
#include "object/sets.h"
#include "tridi/shape.h"
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
    class dynamic_tag { };

    static constexpr auto dummy   = dummy_tag {};
    static constexpr auto proto   = proto_tag {};
    static constexpr auto dynamic = dynamic_tag {};

    static std::chrono::milliseconds const lock_timeout;
    static sl::time const                  default_tick_period;

    basic_entity(basic_entity const &en) noexcept;
    basic_entity(basic_entity &&en) noexcept;
    auto operator=(basic_entity const &en) noexcept -> basic_entity &;
    auto operator=(basic_entity &&en) noexcept -> basic_entity &;

    /*  Prototype initialization.
     */
    explicit basic_entity(proto_tag = proto);

    /*  Dummy initialization.
     */
    explicit basic_entity(dummy_tag);

    /*  Initialize dynamic entity.
     */
    explicit basic_entity(dynamic_tag,
                          sl::time tick_period = default_tick_period);

    virtual ~basic_entity() = default;

    /*  Set the Entity dynamic status.
     *  Thread-safe.
     */
    void set_dynamic(bool is_dynamic);

    /*  Set the Entity tick period.
     *  Thread-safe.
     */
    void set_tick_period(sl::time tick_period);

    /*  Set the Entity clock current time.
     *  Thread-safe.
     */
    void set_clock(sl::time clock_msec);

    /*  Set the Entity clock.
     *  Thread-safe.
     */
    void reset_clock();

    void set_world(ptr_world const &w, sl::index id);
    void reset_world();

    [[nodiscard]] auto index_of(sl::index id) -> sl::index;
    [[nodiscard]] auto get_count() const -> sl::whole;

    [[nodiscard]] auto id_of(sl::index index) -> sl::index;
    [[nodiscard]] auto scale_of(sl::index index) -> sl::index;

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

    [[nodiscard]] auto bytes_get_all() noexcept -> sl::vector<int8_t>;

    void bytes_set(sl::index n, int8_t value) noexcept;

    void bytes_apply_delta(sl::index n, int8_t delta) noexcept;

    void bytes_read(sl::index n, std::span<int8_t> dst) noexcept;

    void bytes_write(sl::index               n,
                     std::span<const int8_t> values) noexcept;

    void bytes_write_delta(sl::index               n,
                           std::span<const int8_t> deltas) noexcept;

    void bytes_erase_delta(sl::index               n,
                           std::span<const int8_t> deltas) noexcept;

    void bytes_resize(sl::whole size) noexcept;

    [[nodiscard]] auto vec_get_size() noexcept -> sl::whole;

    [[nodiscard]] auto vec_get(sl::index n) noexcept -> intval;

    [[nodiscard]] auto vec_get_all() noexcept -> sl::vector<intval>;

    void vec_set(sl::index n, intval value) noexcept;

    void vec_apply_delta(sl::index n, intval delta) noexcept;

    void vec_read(sl::index n, std::span<intval> dst) noexcept;

    void vec_write(sl::index               n,
                   std::span<const intval> values) noexcept;

    void vec_write_delta(sl::index               n,
                         std::span<const intval> deltas) noexcept;

    void vec_erase_delta(sl::index               n,
                         std::span<const intval> deltas) noexcept;

    void vec_resize(sl::whole size) noexcept;

    void vec_add(intval value) noexcept;
    void vec_add_sorted(intval value) noexcept;
    void vec_insert(sl::index n, intval value) noexcept;
    void vec_erase(sl::index n) noexcept;
    void vec_erase_by_value(intval value) noexcept;
    void vec_erase_by_value_sorted(intval value) noexcept;

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
    [[nodiscard]] auto get_tick_period() -> sl::time;

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

    using vsets_row = sl::vector<sets_row>;

    /*  Setup state values.
     */
    void setup_sets(vsets_row const &sets);

    /*  Init state value.
     */
    void init(sl::index n, intval value);

    void bytes_init(sl::index n, int8_t value) noexcept;

    void vec_init(sl::index n, intval value) noexcept;

    void self_destruct(access::world const &w) const;
    void desync();

  private:
    void assign(basic_entity const &en) noexcept;
    void assign(basic_entity &&en) noexcept;

    void sets_invalidate(sl::index n) noexcept;
    void sets_validate_all() noexcept;

    void bytes_invalidate(sl::index n, sl::whole count = 1) noexcept;
    void bytes_validate_all() noexcept;

    void vec_invalidate(sl::index n, sl::whole count = 1) noexcept;
    void vec_validate_all() noexcept;

    struct _range {
      sl::index begin;
      sl::index end;
    };

    void invalidate_range(sl::vector<_range> &ranges,
                          sl::index           n,
                          sl::whole           count) noexcept;

    std::shared_timed_mutex m_lock;
    std::weak_ptr<world>    m_world;
    vsets_row               m_sets;
    sl::vector<_range>      m_sets_changed;
    sl::vector<bytes_row>   m_bytes;
    sl::vector<_range>      m_bytes_changed;
    sl::vector<vec_row>     m_vec;
    sl::vector<_range>      m_vec_changed;
    sl::time                m_clock = {};
    sl::index               m_id    = id_undefined;
  };
}

#include "basic_entity.impl.h"

#endif
