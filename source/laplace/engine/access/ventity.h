#pragma once

#include "entity.h"

namespace laplace::engine::access
{
    class ventity
    {
    public:
        ventity(const ventity &) = delete;
        ventity(ventity &&) = default;
        ventity &operator =(const ventity &) = delete;
        ventity &operator =(ventity &&) = delete;

        ventity(vptr_entity ents, mode access_mode);
        ~ventity() = default;

        auto size() const -> size_t;
        auto operator [](size_t i) const -> entity;

    private:
        vptr_entity m_entities;
        mode        m_mode      = forbidden;
    };
}

#include "ventity.impl.h"
