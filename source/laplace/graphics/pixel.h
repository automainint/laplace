#pragma once

#include <vector>
#include <cstdint>

namespace laplace::graphics
{
    union pixel
    {
        uint32_t    as_uint = 0;
        float       as_float;

        struct
        {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
            uint8_t alpha;
        };
    };

    using cref_pixel = const pixel &;
    using vpixel = std::vector<pixel>;
    using cref_vpixel = const vpixel &;

    constexpr auto rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) -> pixel;
}

#include "pixel.impl.h"
