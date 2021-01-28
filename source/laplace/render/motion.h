#pragma once

#include "defs.h"
#include <vector>
#include <memory>

namespace laplace::render
{
    class model;

    using ptr_model = std::shared_ptr<model>;

    class motion
    {
    public:
        using cref_motion = const motion &;

        motion();
        ~motion();

        void setup(ptr_model source);
        void solve(timeval time);
        void blend(cref_motion state, realmax_t factor);

    private:
        ptr_model m_source;
    };

    using cref_motion = motion::cref_motion;
    using ref_motion = motion &;
    using ptr_motion = std::shared_ptr<motion>;
}
