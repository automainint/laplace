#pragma once

#include <cstdint>

namespace laplace::engine::access
{
    enum mode : uint8_t
    {
        forbidden,
        sync,
        async
    };
}
