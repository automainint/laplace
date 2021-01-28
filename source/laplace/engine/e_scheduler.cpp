#include "scheduler.h"
#include "world.h"
#include "impact.h"

using namespace laplace;
using namespace engine;
using namespace std;

scheduler::scheduler(ref_world w) : m_world(w) { }

scheduler::~scheduler()
{
    set_done();

    for (size_t i = 0; i < m_threads.size(); i++)
        m_threads[i].join();
}

void scheduler::schedule(size_t delta)
{
    auto _ul_ex = unique_lock(m_extra_lock);
    auto _ul    = unique_lock(m_lock);

    m_tick_count += delta;
    _ul.unlock();

    m_sync.notify_all();
}

void scheduler::join()
{
    auto _ul_ex = unique_lock(m_extra_lock);
    auto _ul    = unique_lock(m_lock);

    if (!m_threads.empty())
    {
        m_sync.wait(_ul, [=] { 
            return m_tick_count == 0 || m_threads.empty(); 
        });
    }
}

void scheduler::set_thread_count(size_t thread_count)
{
    auto _ul_ex = unique_lock(m_extra_lock);
    auto _ul    = unique_lock(m_lock);

    size_t n = m_threads.size();

    if (thread_count < n)
    {
        m_done = true;
        _ul.unlock();

        m_sync.notify_all();

        for (size_t i = 0; i < m_threads.size(); i++)
        {
            m_threads[i].join();
        }

        _ul.lock();
        m_done = false;

        n = 0;
    }
    else if (thread_count > n)
    {
        m_threads.resize(thread_count);
    }

    for (size_t i = n; i < m_threads.size(); i++)
    {
        m_threads[i] = thread([=] {
            this->tick_thread();
        });
    }
}

auto scheduler::get_thread_count() -> size_t
{
    auto _ul_ex = unique_lock(m_extra_lock);
    auto _ul    = unique_lock(m_lock);

    return m_threads.size();
}

void scheduler::set_done()
{
    auto _ul_ex = unique_lock(m_extra_lock);
    auto _ul    = unique_lock(m_lock);

    m_done = true;
    _ul.unlock();

    m_sync.notify_all();
}

void scheduler::sync(function<void ()> fn)
{
    auto _ul = unique_lock(m_lock);

    m_in++;

    if (m_in >= m_threads.size())
    {
        m_out = 0;
        _ul.unlock();

        fn();

        m_sync.notify_all();

        _ul.lock();
    }
    else
    {
        m_sync.wait(_ul, [=] { 
            return m_in >= m_threads.size();
        });
    }

    m_out++;

    if (m_out >= m_threads.size())
    {
        m_in = 0;
        _ul.unlock();

        m_sync.notify_all();
    }
    else
    {
        m_sync.wait(_ul, [=] {
            return m_out >= m_threads.size();
        });
    }
}

void scheduler::tick_thread()
{
    auto _ul = unique_lock(m_lock);

    while (m_sync.wait(_ul, [=] { return m_tick_count > 0 || m_done; }), !m_done)
    {
        while (m_tick_count > 0)
        {
            _ul.unlock();

            while (!m_world.no_queue())
            {
                /*  Execute the sync queue.
                 */

                sync([=] {
                    while (auto ev = m_world.next_async_impact())
                    {
                        ev->perform({ m_world, access::sync });
                    }

                    m_world.clean_sync_queue();
                });

                /*  Execute the async queue.
                 */

                while (auto ev = m_world.next_async_impact())
                {
                    ev->perform({ m_world, access::async });
                }

                sync([=] {
                    m_world.clean_async_queue();
                });
            }

            /*  Update the dynamic entities.
             */

            while (auto en = m_world.next_dynamic_entity())
            {
                if (en->clock())
                {
                    en->tick({ m_world, access::async });
                }
            }

            sync([=] {
                m_world.reset_index();
            });

            /*  Adjust all the entities.
             */

            while (auto en = m_world.next_entity())
            {
                en->adjust();
            }

            sync([=] { 
                m_world.reset_index();

                m_tick_count--;
            });

            _ul.lock();
        }

        _ul.unlock();

        m_sync.notify_all();

        _ul.lock();
    }
}
