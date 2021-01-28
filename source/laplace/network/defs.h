#pragma once

#include <cstdint>

namespace laplace::network
{
    static constexpr size_t     max_chunk_size  = 0x7fffffff;
    static constexpr auto       localhost       = "127.0.0.1";
    static constexpr uint16_t   any_port        = 0;

    enum io_mode
    {
        async,
        sync
    };
}
