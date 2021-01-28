#pragma once

#include "world.h"
#include "impact.h"

namespace laplace::engine
{
    /*  World live execution solver.
     */
    class solver
    {
    public:
        solver();
        ~solver();

        void set_world(ptr_world w);
        void reset_world();

        /*  Add Impact to queue. Adjust the Impact
         *  time due performing if needed.
         *
         *  If an Impact time is set, it can change
         *  the previous World state, which will
         *  cause rewind and recalculating of the
         *  entire timeline.
         */
        void apply(ptr_impact imp);

        void rewind_to(uint64_t time);

        /*  Equals to schedule and join.
         */
        void solve(uint64_t delta);

        void schedule(uint64_t delta);
        void join();

        auto get_time() const -> uint64_t;
        auto get_position() const -> size_t;

        void set_seed(seed_type seed);
        auto get_seed() const -> seed_type;

        void clear_history();
        auto get_history_count() const -> size_t;
        auto get_history(size_t index) const -> ptr_impact;

    private:
        void adjust(uint64_t time);

        uint64_t   m_time;
        size_t      m_position;
        seed_type   m_seed;
        ptr_world   m_world;
        vptr_impact m_history;
    };

    using ptr_solver = std::shared_ptr<solver>;
}
