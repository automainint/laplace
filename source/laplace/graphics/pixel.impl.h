#pragma once

namespace laplace::graphics
{
    constexpr auto rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) -> pixel
    {
        pixel p;
        p.red = r;
        p.green = g;
        p.blue = b;
        p.alpha = a;
        return p;
    }
}
