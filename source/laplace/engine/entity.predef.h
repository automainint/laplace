#pragma once

#include <memory>
#include <vector>

namespace laplace::engine
{
    class entity;

    using ref_entity    = entity &;
    using cref_entity   = const entity &;

    using ptr_entity    = std::shared_ptr<entity>;
    using vptr_entity   = std::vector<ptr_entity>;

    static constexpr uint64_t   time_undefined = -1;
    static constexpr size_t     id_undefined    = -1;
}
