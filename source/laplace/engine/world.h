/*  laplace/engine/world.h
 *
 *      World compute kernel and Entity pool. Full thread-safe.
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

#ifndef laplace_engine_world_h
#define laplace_engine_world_h

#include "../platform/thread.h"
#include "basic_entity.h"
#include "basic_impact.predef.h"
#include "scheduler.h"
#include <functional>
#include <random>
#include <shared_mutex>

namespace laplace::engine {
  class world : public std::enable_shared_from_this<world> {
  public:
    static constexpr bool default_allow_relaxed_spawn = false;

    world(const world &) = delete;
    auto operator=(const world &) -> world & = delete;

    world();
    ~world() = default;

    auto reserve(size_t id) -> size_t;
    void emplace(ptr_entity ent, size_t id);
    auto spawn(ptr_entity ent, size_t id) -> size_t;
    void remove(size_t id);
    void respawn(size_t id);
    void clear();

    void desync();

    /*  Impact will be performed due live loop.
     */
    void queue(ptr_impact ev);

    /*  World live loop tick.
     *  Shedule & join.
     */
    void tick(uint64_t delta);

    /*  Schedule the live loop.
     */
    void schedule(uint64_t delta);

    /*  Join the live loop thread.
     */
    void join();

    void set_thread_count(size_t thread_count);
    auto get_thread_count() -> size_t;

    void set_root(size_t id_root);
    auto get_root() -> size_t;

    void allow_relaxed_spawn(bool is_allowed);
    auto is_relaxed_spawn_allowed() -> bool;

    auto get_random() -> ref_rand;
    auto get_entity(size_t id) -> ptr_entity;

    auto is_desync() -> bool;

    void clean_sync_queue();
    void clean_async_queue();
    void reset_index();
    auto no_queue() -> bool;
    auto next_sync_impact() -> ptr_impact;
    auto next_async_impact() -> ptr_impact;
    auto next_dynamic_entity() -> ptr_entity;
    auto next_entity() -> ptr_entity;

  private:
    void locked_desync();
    void locked_add_dynamic(size_t id);
    void locked_erase_dynamic(size_t id);

    std::shared_mutex          m_lock;
    std::unique_ptr<scheduler> m_scheduler;

    bool   m_allow_relaxed_spawn = default_allow_relaxed_spawn;
    bool   m_desync              = false;
    size_t m_root                = id_undefined;
    size_t m_next_id             = 0;
    size_t m_index               = 0;

    eval::random m_rand;
    vuint        m_dynamic_ids;
    vptr_entity  m_entities;
    vptr_impact  m_queue;
    vptr_impact  m_sync_queue;
  };
}

#endif
