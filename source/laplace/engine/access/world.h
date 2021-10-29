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
     *  Async.
     */
    void desync() const;

    /*  Reserve an entity id.
     *  Sync.
     */
    auto reserve(sl::index id) const -> sl::index;

    /*  Spawn a new entity. Allow to use already taken id.
     *  Sync.
     */
    void emplace(ptr_entity ent, sl::index id) const;

    /*  Spawn a new entity.
     *  Sync.
     */
    auto spawn(ptr_entity ent, sl::index id) const -> sl::index;

    /*  Remove an existing entity.
     *  Sync.
     */
    void remove(sl::index id) const;

    /*  Respawn an entity.
     *  Sync.
     */
    void respawn(sl::index id) const;

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
    void set_root(sl::index id_root) const;

    /*  Get the root entity id.
     *  Read.
     */
    [[nodiscard]] auto get_root() const -> sl::index;

    /*  If the world has an entity.
     *  Read.
     */
    [[nodiscard]] auto has_entity(sl::index id) const -> bool;

    /*  Get an entity.
     *  Read.
     */
    [[nodiscard]] auto get_entity(sl::index id) const -> access::entity;

    /*  Generate a random number.
     *  Sync.
     */
    template <typename dist_>
    [[nodiscard]] auto random(dist_ &dist) const ->
        typename dist_::result_type;

    /*  Generate a random number sequence.
     *  Sync.
     */
    template <typename dist_>
    [[nodiscard]] auto random(dist_ &dist, sl::whole count) const ->
        sl::vector<typename dist_::result_type>;

  private:
    auto get_random_engine() const -> ref_rand;

    std::reference_wrapper<engine::world> m_world;

    mode m_mode = forbidden;
  };
}

#include "world.impl.h"

#endif
