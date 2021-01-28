#pragma once

#include <laplace/engine/protocol/slot_create.h>

namespace quadwar_app
{
    using namespace laplace;

    class qw_slot_create : public engine::protocol::slot_create
    {
    public:
        ~qw_slot_create() override = default;

        constexpr qw_slot_create(size_t index, uint64_t time, size_t id_actor, bool is_local)
            : slot_create(index, time, id_actor, is_local)
        { }

        void perform(engine::access::world w) const override;

        static inline auto decode(cref_vbyte seq)
        {
            return qw_slot_create {
                get_index(seq),
                get_time(seq),
                get_slot_actor(seq),
                get_local_flag(seq) > 0
            };
        }
    };
}
