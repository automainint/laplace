#pragma once

#include "../impact.h"

namespace laplace::engine::protocol
{
    class slot_remove : public sync_impact
    {
    public:
        enum encoding_offset : size_t
        {
            n_slot_actor = 18
        };

        static constexpr uint16_t   id      = ids::slot_remove;
        static constexpr size_t     size    = 26;

        ~slot_remove() override = default;

        constexpr slot_remove()
        {
            set_size(size);
        }

        constexpr slot_remove(size_t index, uint64_t time, size_t id_actor)
        {
            set_order({ index });
            set_time(time);
            set_actor(id_actor);
            set_size(size);
        }

        void perform(access::world w) const override;
        
        inline void encode_to(std::span<uint8_t> bytes) const final
        {
            write_bytes(
                bytes,
                id,
                get_index64(),
                get_time64(),
                get_actor64()
            );
        }

        static constexpr auto scan(cref_vbyte seq)
        {
            return seq.size() == size && get_id(seq) == id;
        }

        static inline auto decode(cref_vbyte seq)
        {
            return slot_remove {
                get_index(seq),
                get_time(seq),
                get_slot_actor(seq)
            };
        }

        static constexpr auto get_slot_actor(cref_vbyte seq) -> size_t
        {
            return as_index(rd<uint64_t>(seq, n_slot_actor));
        }
    };
}
