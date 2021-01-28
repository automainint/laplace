#pragma once

#include <laplace/engine/object/sets.h>

namespace quadwar_app::sets
{
    using namespace laplace::engine::object::sets;

    enum set_id : size_t
    {
        player_name = _native_count,

        slot_count,
        slot_get,
        slot_create,
        slot_remove
    };
}
