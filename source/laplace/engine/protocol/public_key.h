#pragma once

#include "../impact.h"

namespace laplace::engine::protocol
{
    class public_key final : public impact
    {
    public:
        enum encoding_offset : size_t
        {
            n_cipher    = 2,
            n_key       = 4
        };

        static constexpr size_t     max_key_size = 1024;
        static constexpr uint16_t   id = ids::public_key;

        using key_type = std::array<uint8_t, max_key_size>;

        ~public_key() final = default;

        constexpr public_key()
        {
            set_size(n_key);
        }

        constexpr public_key(uint16_t cipher_id, cref_vbyte key)
        {
            m_cipher_id = cipher_id;
            m_key_size  = std::min(key.size(), max_key_size);

            set_size(n_key + m_key_size);

            std::copy(key.begin(), key.begin() + m_key_size, m_key.begin());
        }

        static constexpr auto get_cipher(cref_vbyte seq)
        {
            return rd<uint16_t>(seq, n_cipher);
        }

        static constexpr auto get_key(cref_vbyte seq)
        {
            return
                seq.size() > n_key ?
                std::span<const uint8_t>(seq.begin() + n_key, seq.end()) :
                std::span<const uint8_t> { };
        }

        inline void encode_to(std::span<uint8_t> bytes) const final
        {
            write_bytes(
                bytes,
                id,
                m_cipher_id,
                std::span<const uint8_t>(m_key.begin(), m_key.begin() + m_key_size)
            );
        }

        static constexpr auto scan(cref_vbyte seq) -> bool
        {
            return seq.size() >= n_key && rd<uint16_t>(seq, 0) == id;
        }

        static inline auto decode(cref_vbyte seq)
        {
            return public_key {
                get_cipher(seq),
                get_key(seq)
            };
        }

    private:
        uint16_t    m_cipher_id = ids::cipher_plain;
        size_t      m_key_size  = 0;
        key_type    m_key       = { };
    };
}
