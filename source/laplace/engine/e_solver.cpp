#include "solver.h"
#include <chrono>
#include <algorithm>

using namespace laplace::engine;
using namespace std;

solver::solver()
{
    m_time = 0;
    m_position = 0;

    m_seed = static_cast<seed_type>(chrono::system_clock::now().time_since_epoch().count());
}

solver::~solver() { }

void solver::set_world(ptr_world w)
{
    m_world = w;
    m_world->get_random().seed(m_seed);
}

void solver::reset_world()
{
    m_world.reset();
}

void solver::apply(ptr_impact imp)
{
    if (imp)
    {
        if (imp->get_time() == time_undefined)
        {
            imp->set_time(m_time);
        }
        
        if (imp->get_time() < m_time)
        {
            auto time = m_time;
            rewind_to(imp->get_time());

            m_history.emplace(m_history.begin() + m_position, imp);

            rewind_to(time);
        }
        else
        {
            auto op = [](const ptr_impact &a, uint64_t b) -> bool {
                return a->get_time() < b;
            };

            auto it = lower_bound(m_history.begin() + m_position, m_history.end(), imp->get_time(), op);

            m_history.emplace(it, imp);
        }
    }
}

void solver::rewind_to(uint64_t time)
{
    adjust(time);
    join();
}

void solver::solve(uint64_t delta)
{
    schedule(delta);
    join();
}

void solver::schedule(uint64_t delta)
{
    adjust(m_time + delta);
}

void solver::join()
{
    if (m_world)
        m_world->join();
}

auto solver::get_time() const -> uint64_t
{
    return m_time;
}

auto solver::get_position() const -> size_t
{
    return m_position;
}

void solver::set_seed(seed_type seed)
{
    m_seed = seed;

    if (m_world)
    {
        m_world->get_random().seed(m_seed);
    }
}

auto solver::get_seed() const -> seed_type
{
    return m_seed;
}

void solver::clear_history()
{
    m_time = 0;
    m_position = 0;

    m_history.clear();
}

auto solver::get_history_count() const -> size_t
{
    return m_history.size();
}

auto solver::get_history(size_t index) const -> ptr_impact
{
    return index < m_history.size() ? m_history[index] : ptr_impact();
}

void solver::adjust(uint64_t time)
{
    if (m_world && m_time != time)
    {
        if (time < m_time)
        {
            m_world->clear();
            m_world->get_random().seed(m_seed);

            m_time = 0;
            m_position = 0;
        }

        auto op = [](const shared_ptr<impact> &a, uint64_t b) -> bool {
            return a->get_time() < b;
        };

        auto i_end = lower_bound(m_history.begin() + m_position, m_history.end(), time, op);

        auto t0 = m_time;

        for (auto i = m_history.begin() + m_position; i != i_end; i++)
        {
            auto t1 = (*i)->get_time();

            if (t0 < t1)
            {
                m_world->tick(t1 - t0);
                t0 = t1;
            }

            m_world->queue(*i);
        }

        if (t0 < time)
        {
            m_world->schedule(time - t0);
        }

        m_time = time;
        m_position = i_end - m_history.begin();
    }
}
