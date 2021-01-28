#pragma once

#include "../entity.h"
#include "../impact.h"

namespace laplace::engine::object
{
    /*  Timer entity.
     *
     *  Dynamic, self-destructive.
     */
    class timer final : public entity
    {
    public:
        timer(impact_gen gen, uint64_t period, size_t count = 1);
        ~timer() final = default;

        void tick(access::world w) final;

    private:
        timer(proto_tag);

        static timer m_proto;

        size_t m_count = 1;
        impact_gen m_gen;
    };
}
