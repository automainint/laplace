#pragma once

template <typename impact_type>
inline auto laplace::engine::factory::make(cref_vbyte seq) -> laplace::engine::ptr_impact
{
    return std::make_shared<impact_type>(std::move(impact_type::decode(seq)));
}
