#pragma once

#include "ventity.h"
#include "../impact.predef.h"
#include "../world.predef.h"

namespace laplace::engine::access
{
    class world
    {
    public:
        world(const world &) = delete;
        world(world &&);
        auto operator =(const world &) -> world & = delete;
        auto operator =(world &&) -> world & = delete;

        world(ref_world w, mode access_mode);
        ~world() = default;

        /*  Set the desync flag.
         */
        void desync();

        /*  Reserve an entity id.
         *  Sync.
         */
        auto reserve(size_t id) -> size_t;

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
        auto random(distribution &dist) -> typename distribution::result_type;

    private:
        auto get_random_engine() -> ref_rand;

        ref_world   m_world;
        mode        m_mode  = forbidden;
    };
}

#include "world.impl.h"
