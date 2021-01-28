#pragma once

#include "world.predef.h"
#include <functional>
#include <thread>
#include <condition_variable>

namespace laplace::engine
{
    /*  The World multithreading scheduler.
     *  Full thread-safe.
     */
    class scheduler
    {
    public:
        scheduler(ref_world w);
        ~scheduler();

        void schedule(size_t delta);
        void join();

        void set_thread_count(size_t thread_count);
        auto get_thread_count() -> size_t;

    private:
        void set_done();
        void sync(std::function<void ()> fn);
        void tick_thread();

        ref_world m_world;

        std::mutex                  m_lock;
        std::mutex                  m_extra_lock;
        std::condition_variable     m_sync;
        std::vector<std::thread>    m_threads;

        bool    m_done          = false;
        size_t  m_in            = 0;
        size_t  m_out           = 0;
        size_t  m_tick_count    = 0;
    };
}
