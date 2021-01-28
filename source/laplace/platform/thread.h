#pragma once

#include <thread>

namespace laplace::platform
{
    namespace priority
    {
        enum : int
        {
            idle = 1,
            lowest,
            low,
            normal,
            high,
            highest,
            critical
        };
    }
}
