#pragma once

#include "../impact.h"

namespace laplace::engine::action
{
    class remove final : public sync_impact
    {
    public:
        remove(size_t id_entity);
        ~remove() final = default;

        void perform(access::world w) const final;

    private:
        size_t m_id_entity;
    };
}
