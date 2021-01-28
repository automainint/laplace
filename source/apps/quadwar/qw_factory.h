#pragma once

#include <laplace/engine/factory.h>

namespace quadwar_app
{
    using namespace laplace;

    class qw_factory : public engine::factory
    {
    public:
        ~qw_factory() override = default;

        auto decode(cref_vbyte seq) -> engine::ptr_impact override;

    private:
    };
}
