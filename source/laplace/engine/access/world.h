/*  laplace/engine/access/world.h
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

#ifndef __laplace__engine_access_world__
#define __laplace__engine_access_world__

#include "../basic_impact.predef.h"
#include "../world.predef.h"
#include "ventity.h"

namespace laplace::engine::access {
  class world {
  public:
    world(const world &) = delete;
    auto operator=(const world &) -> world & = delete;

    [[nodiscard]] world(world &&w) noexcept;
    [[nodiscard]] world(ref_world w, mode access_mode);

    auto operator=(world &&w) noexcept -> world &;

    ~world() = default;

    /*  Set the desync flag.
     */
    void desync();

    /*  Reserve an entity id.
     *  Sync.
     */
    auto reserve(size_t id) -> size_t;

    /*  Spawn a new entity. Allow to use already taken id.
     *  Sync.
     */
    void emplace(ptr_entity ent, size_t id);

    /*  Spawn a new entity.
     *  Sync.
     */
    auto spawn(ptr_entity ent, size_t id) -> size_t;

    /*  Remove an existing entity.
     *  Sync.
     */
    void remove(size_t id);

    /*  Respawn an entity.
     *  Sync.
     */
    void respawn(size_t id);

    /*  Remove all entities.
     *  Sync.
     */
    void clear();

    /*  Queue an event.
     *  Async.
     */
    void queue(ptr_impact ev);

    /*  Set the root entity.
     *  Sync.
     */
    void set_root(ptr_entity root);

    /*  Get the root entity.
     *  Async.
     */
    auto get_root() -> access::entity;

    /*  If the world has an entity.
     *  Async.
     */
    auto has_entity(size_t id) -> bool;

    /*  Get an entity.
     *  Async.
     */
    auto get_entity(size_t id) -> access::entity;

    /*  Select entities.
     *  Async.
     */
    auto select(condition op) -> access::ventity;

    /*  Select dynamic entities.
     *  Async.
     */
    auto select_dynamic(condition op) -> access::ventity;

    /*  Generate a random number.
     *  Sync.
     */
    template <typename distribution>
    auto random(distribution &dist) ->
        typename distribution::result_type;

  private:
    auto get_random_engine() -> ref_rand;

    std::reference_wrapper<engine::world> m_world;
    mode                                  m_mode = forbidden;
  };
}

#include "world.impl.h"

#endif
