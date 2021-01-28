#include "dh_rabbit.h"
#include "protocol/prime.h"

using namespace laplace;
using namespace engine;
using namespace std;

void dh_rabbit::init()
{
    wc_InitRng(&m_random);
    wc_InitDhKey(&m_key);

    auto status = wc_DhSetKey(
        &m_key, protocol::prime, sizeof protocol::prime,
        protocol::generator, sizeof protocol::generator);

    if (status != 0)
    {
        cerr << __FUNCTION__ << ": wc_DhSetKey failed." << endl;
        m_is_ok = false;
        return;
    }
}

dh_rabbit::dh_rabbit()
{
    init();

    if (m_is_ok)
    {
        auto status = wc_DhGenerateKeyPair(
            &m_key, &m_random,
            m_private.data(), &m_private_size,
            m_public.data(), &m_public_size);

        if (status != 0)
        {
            cerr << __FUNCTION__ << ": wc_DhGenerateKeyPair failed." << endl;
            m_is_ok = false;
            return;
        }
    }
}

dh_rabbit::dh_rabbit(dh_rabbit &&r) noexcept
{
    init();

    if (m_is_ok)
    {
        m_private_size  = r.m_private_size;
        m_public_size   = r.m_public_size;
        m_mutual_size   = r.m_mutual_size;
        m_private       = r.m_private;
        m_public        = r.m_public;
        m_mutual        = r.m_mutual;
        m_dh_rabbit        = r.m_dh_rabbit;
    }
}

auto dh_rabbit::operator =(dh_rabbit &&r) noexcept -> dh_rabbit &
{
    m_private_size  = r.m_private_size;
    m_public_size   = r.m_public_size;
    m_mutual_size   = r.m_mutual_size;
    m_private       = r.m_private;
    m_public        = r.m_public;
    m_mutual        = r.m_mutual;
    m_dh_rabbit        = r.m_dh_rabbit;

    return *this;
}

dh_rabbit::~dh_rabbit()
{
    wc_FreeRng(&m_random);
    wc_FreeDhKey(&m_key);
}

auto dh_rabbit::get_public_key() const -> cref_vbyte
{
    return m_public;
}

void dh_rabbit::set_remote_key(cref_vbyte key)
{
    if (!m_is_ok) return;

    m_is_ready      = false;
    m_mutual_size   = key_size;

    auto status = wc_DhAgree(
        &m_key,
        m_mutual.data(), &m_mutual_size,
        m_private.data(), m_private_size,
        key.data(), key.size());

    if (status != 0)
    {
        cerr << __FUNCTION__ << ": wc_DhAgree failed." << endl;
        return;
    }

    status = wc_RabbitSetKey(
        &m_dh_rabbit,
        m_mutual.data(),
        m_mutual.data() + 16);

    if (status != 0)
    {
        cerr << __FUNCTION__ << ": wc_RabbitSetKey failed." << endl;
        return;
    }

    m_is_ready = true;
}

auto dh_rabbit::encrypt(cref_vbyte bytes) -> vbyte
{
    if (!is_ready() || bytes.empty())
    {
        return vbyte(bytes.begin(), bytes.end());
    }

    vbyte result(bytes.size());

    auto status = wc_RabbitProcess(
        &m_dh_rabbit, result.data(),
        bytes.data(), bytes.size());

    if (status != 0)
    {
        cerr << __FUNCTION__ << ": wc_RabbitProcess failed." << endl;
        return { };
    }

    return result;
}

auto dh_rabbit::decrypt(cref_vbyte bytes) -> vbyte
{
    return encrypt(bytes);
}

auto dh_rabbit::is_ready() -> bool
{
    return m_is_ok && m_is_ready;
}
