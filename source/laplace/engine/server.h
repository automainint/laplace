#pragma once

#include "factory.h"
#include "impact.h"
#include "solver.h"

namespace laplace::engine
{
    enum class server_state
    {
        prepare,
        action,
        pause
    };

    class server
    {
    public:
        static constexpr uint64_t default_tick_duration_msec = 10;

        server();
        virtual ~server() = default;

        void set_factory(ptr_factory fac);

        virtual void queue(cref_vbyte seq);
        virtual void tick(size_t delta_msec);

        auto get_factory() const -> ptr_factory;
        auto get_solver() const -> ptr_solver;
        auto get_world() const -> ptr_world;

        auto get_ping() const -> uint64_t;

        auto get_state() const -> server_state;
        auto get_tick_duration() -> size_t;

    protected:
        void set_tick_duration(uint64_t tick_duration_msec);
        void set_random_seed(seed_type seed);

        void set_ping(uint64_t ping_msec);
        void set_state(server_state state);

        /*  Update tick timer. Returns time
         *  delta in ticks.
         */
        auto adjust_delta(size_t delta_msec) -> uint64_t;

    private:
        ptr_factory m_factory;
        ptr_solver  m_solver    = std::make_shared<solver>();
        ptr_world   m_world     = std::make_shared<world>();

        uint64_t       m_ping_msec             = 0;
        uint64_t       m_tick_clock_msec       = 0;
        uint64_t       m_tick_duration_msec    = default_tick_duration_msec;
        server_state    m_state                 = server_state::prepare;
    };

    using ptr_server = std::shared_ptr<server>;
}
