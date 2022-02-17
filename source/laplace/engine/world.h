/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_engine_world_h
#define laplace_engine_world_h

#include "../platform/thread.h"
#include "basic_entity.h"
#include "basic_impact.h"
#include "scheduler.h"
#include <functional>
#include <random>
#include <shared_mutex>

namespace laplace::engine {
  class world : public std::enable_shared_from_this<world> {
  public:
    static bool const     default_allow_relaxed_spawn;
    static uint64_t const default_seed;

    world(world const &) = delete;
    auto operator=(world const &) -> world & = delete;

    world(world &&other) noexcept;
    auto operator=(world &&other) noexcept -> world &;

    world() noexcept;
    ~world() noexcept;

    auto reserve(sl::index id) noexcept -> sl::index;
    void emplace(ptr_entity const &ent, sl::index id) noexcept;
    auto spawn(ptr_entity const &ent, sl::index id) noexcept
        -> sl::index;
    void remove(sl::index id) noexcept;
    void respawn(sl::index id) noexcept;
    void clear() noexcept;

    void desync() noexcept;

    /*  Impact will be performed due live loop.
     */
    void queue(ptr_impact ev) noexcept;

    /*  World live loop tick.
     *  Schedule & join.
     */
    void tick(sl::time delta) noexcept;

    /*  Schedule the live loop.
     */
    void schedule(sl::time delta) noexcept;

    /*  Join the live loop thread.
     */
    void join() noexcept;

    void set_thread_count(sl::whole thread_count) noexcept;

    [[nodiscard]] auto get_thread_count() noexcept -> sl::whole;

    void               set_root(sl::index id_root) noexcept;
    [[nodiscard]] auto get_root() noexcept -> sl::index;

    void               allow_relaxed_spawn(bool is_allowed) noexcept;
    [[nodiscard]] auto is_relaxed_spawn_allowed() noexcept -> bool;

    [[nodiscard]] auto get_random() noexcept -> ref_rand;
    [[nodiscard]] auto get_entity(sl::index id) noexcept
        -> ptr_entity;

    [[nodiscard]] auto is_desync() noexcept -> bool;

    /*  TODO
     *  Sort queue & clean queue order.
     *  Coroutines support.
     *
     *    Use mutual queue for sync & async impacts.
     *
     *    Use swap queue for adding new impacts during queue
     *    execution.
     */
    void clean_sync_queue() noexcept;
    void clean_async_queue() noexcept;
    void reset_index() noexcept;

    [[nodiscard]] auto no_queue() noexcept -> bool;
    [[nodiscard]] auto next_sync_impact() noexcept -> ptr_impact;
    [[nodiscard]] auto next_async_impact() noexcept -> ptr_impact;
    [[nodiscard]] auto next_dynamic_entity() noexcept -> ptr_entity;
    [[nodiscard]] auto next_entity() noexcept -> ptr_entity;

  private:
    [[nodiscard]] auto check_scheduler() noexcept -> bool;

    void locked_desync() noexcept;
    void locked_add_dynamic(sl::index id) noexcept;
    void locked_erase_dynamic(sl::index id) noexcept;

    void locked_assign(world &&other) noexcept;

    std::shared_mutex          m_lock;
    std::unique_ptr<scheduler> m_scheduler;

    bool      m_allow_relaxed_spawn = default_allow_relaxed_spawn;
    bool      m_desync              = false;
    sl::index m_root                = id_undefined;
    sl::index m_next_id             = 0;
    sl::index m_index               = 0;

    eval::random m_rand = eval::random { default_seed };

    sl::vector<sl::index> m_dynamic_ids;
    vptr_entity           m_entities;
    vptr_impact           m_queue;
    vptr_impact           m_sync_queue;
  };
}

#endif
