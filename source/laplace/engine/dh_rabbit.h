#pragma once

#include "../core/defs.h"
#define WC_NO_HARDEN
#include <wolfssl/wolfcrypt/dh.h>
#include <wolfssl/wolfcrypt/rabbit.h>
#include <wolfssl/wolfcrypt/random.h>

namespace laplace::engine
{
    class dh_rabbit
    {
    public:
        dh_rabbit(const dh_rabbit &) = delete;
        auto operator =(const dh_rabbit &) -> dh_rabbit & = delete;

        dh_rabbit();
        dh_rabbit(dh_rabbit &&r) noexcept;
        auto operator =(dh_rabbit &&r) noexcept -> dh_rabbit &;

        ~dh_rabbit();

        auto get_public_key() const -> cref_vbyte;
        void set_remote_key(cref_vbyte key);

        auto encrypt(cref_vbyte bytes) -> vbyte;
        auto decrypt(cref_vbyte bytes) -> vbyte;

        auto is_ready() -> bool;

    private:
        void init();

        static constexpr size_t key_size = 256;

        using key_bytes = std::array<uint8_t, key_size>;

        bool        m_is_ok         = true;
        bool        m_is_ready      = false;

        WC_RNG      m_random;
        DhKey       m_key;

        uint32_t    m_public_size   = key_size;
        uint32_t    m_private_size  = key_size;
        uint32_t    m_mutual_size   = 0;

        key_bytes   m_public        = { 0 };
        key_bytes   m_private       = { 0 };
        key_bytes   m_mutual        = { 0 };

        Rabbit      m_dh_rabbit        = { 0 };
    };
}
