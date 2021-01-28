#pragma once

#include <laplace/engine/entity.h>

namespace quadwar_app
{
    using namespace laplace;

    class landscape : public engine::entity
    {
    public:
        landscape();

    private:
        landscape(int);

        static landscape m_proto;
    };
}
