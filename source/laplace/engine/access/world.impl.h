#pragma once

namespace laplace::engine
{
    template <typename distribution>
    inline auto access::world::random(distribution &dist) -> typename distribution::result_type
    {
        auto result = distribution::result_type(0);

        if (this->m_mode == access::sync)
        {
            result = dist(get_random_engine());
        }

        return result;
    }
}
