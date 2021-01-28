#include "server.h"
#include "factory.h"
#include "../core/utils.h"

using namespace laplace;
using namespace engine;
using namespace std;

server::server()
{
    m_solver->set_world(m_world);
}

void server::set_factory(ptr_factory fac)
{
    m_factory = fac;
}

void server::queue(cref_vbyte seq)
{
    if (m_factory)
    {
        m_solver->apply(m_factory->decode(seq));
    }
}

void server::tick(size_t delta_msec)
{
    if (m_state == server_state::action)
    {
        m_solver->solve(adjust_delta(delta_msec));
    }
}

auto server::get_factory() const -> ptr_factory
{
    return m_factory;
}

auto server::get_solver() const -> ptr_solver
{
    return m_solver;
}

auto server::get_world() const -> ptr_world
{
    return m_world;
}

auto server::get_ping() const -> uint64_t
{
    return m_ping_msec;
}

auto server::get_state() const -> server_state
{
    return m_state;
}

auto server::get_tick_duration() -> size_t
{
    return m_tick_duration_msec;
}

void server::set_tick_duration(uint64_t tick_duration_msec)
{
    m_tick_duration_msec = tick_duration_msec;
}

void server::set_random_seed(seed_type seed)
{
    m_solver->set_seed(seed);
}

void server::set_ping(uint64_t ping_msec)
{
    m_ping_msec = ping_msec;
}

void server::set_state(server_state state)
{
    m_state = state;
}

auto server::adjust_delta(size_t delta_msec) -> uint64_t
{
    uint64_t delta = 0;

    if (m_tick_duration_msec > 0)
    {
        delta = (m_tick_clock_msec + delta_msec) / m_tick_duration_msec;
        m_tick_clock_msec = delta_msec % m_tick_duration_msec;
    }

    return delta;
}
