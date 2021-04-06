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

#ifndef laplace_engine_access_world_h
#define laplace_engine_access_world_h

#include "../basic_impact.predef.h"
#include "../world.predef.h"
#include "entity.h"

namespace laplace::engine::access {
  class world {
  public:
    world(const world &) = delete;
    auto operator=(const world &) -> world & = delete;

    [[nodiscard]] world(world &&w) noexcept;
    [[nodiscard]] world(ref_world w, mode access_mode = read_only);

    auto operator=(world &&w) noexcept -> world &;

    ~world() = default;

    /*  Set the desync flag.
     */
    void desync() const;

    /*  Reserve an entity id.
     *  Sync.
     */
    auto reserve(size_t id) const -> size_t;

    /*  Spawn a new entity. Allow to use already taken id.
     *  Sync.
     */
    void emplace(ptr_entity ent, size_t id) const;

    /*  Spawn a new entity.
     *  Sync.
     */
    auto spawn(ptr_entity ent, size_t id) const -> size_t;

    /*  Remove an existing entity.
     *  Sync.
     */
    void remove(size_t id) const;

    /*  Respawn an entity.
     *  Sync.
     */
    void respawn(size_t id) const;

    /*  Remove all entities.
     *  Sync.
     */
    void clear() const;

    /*  Queue an event.
     *  Async.
     */
    void queue(ptr_impact ev) const;

    /*  Set the root entity id.
     *  Sync.
     */
    void set_root(size_t id_root) const;

    /*  Get the root entity id.
     *  Async.
     */
    [[nodiscard]] auto get_root() const -> size_t;

    /*  If the world has an entity.
     *  Async.
     */
    [[nodiscard]] auto has_entity(size_t id) const -> bool;

    /*  Get an entity.
     *  Async.
     */
    [[nodiscard]] auto get_entity(size_t id) const -> access::entity;

    /*  Generate a random number.
     *  Sync.
     */
    template <typename dist_>
    [[nodiscard]] auto random(dist_ &dist) const ->
        typename dist_::result_type;

  private:
    auto get_random_engine() const -> ref_rand;

    std::reference_wrapper<engine::world> m_world;

    mode m_mode = forbidden;
  };
}

#include "world.impl.h"

#endif
