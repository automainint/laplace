#pragma once

#include "agent.predef.h"
#include "../core/defs.h"

namespace laplace::vm
{
    class environment
    {
    public:
        environment();
        ~environment();

        auto add(size_t unique_id, ptr_agent a) -> size_t;
        auto get_id(size_t unique_id) -> size_t;
        auto perform(size_t id, cref_vbyte args) -> vbyte;

    private:
        std::vector<std::pair<size_t, ptr_agent>> m_ids;
    };
}
