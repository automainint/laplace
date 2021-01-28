#pragma once

#include "../impact.h"

namespace laplace::engine::protocol
{
    class client_ping final : public impact
    {
    public:
        enum encoding_offset : size_t
        {
            n_ping_time = 2
        };

        static constexpr uint16_t   id      = ids::client_ping;
        static constexpr size_t     size    = 10;

        ~client_ping() final = default;

        constexpr client_ping()
        {
            set_size(size);
        }

        constexpr client_ping(uint64_t time_msec)
        {
            set_size(size);

            m_time_msec = time_msec;
        }

        static constexpr auto get_ping_time(cref_vbyte seq)
        {
            return rd<uint64_t>(seq, n_ping_time);
        }

        inline void encode_to(std::span<uint8_t> bytes) const final
        {
            write_bytes(bytes, id, m_time_msec);
        }

        static constexpr auto scan(cref_vbyte seq) -> bool
        {
            return seq.size() == size && get_id(seq) == id;
        }

        static inline auto decode(cref_vbyte seq)
        {
            return client_ping {
                get_ping_time(seq)
            };
        }

    private:
        uint64_t m_time_msec = 0;
    };
}
