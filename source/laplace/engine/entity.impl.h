#pragma once

inline auto laplace::engine::entity::get_by_id(size_t id) -> int64_t
{
    return this->get(this->index_of(id));
}

inline auto laplace::engine::entity::locked_get_by_id(size_t id) const -> int64_t
{
    return this->locked_get(this->index_of(id));
}
