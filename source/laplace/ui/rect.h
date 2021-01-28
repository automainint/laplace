#pragma once

#include <cstdint>

namespace laplace::ui
{
    struct rect
    {
        int x = 0;
        int y = 0;
        size_t width = 0;
        size_t height = 0;
    };

    struct rectf
    {
        float x = 0.f;
        float y = 0.f;
        float width = 0.f;
        float height = 0.f;
    };

    using cref_rect = const rect &;

    auto contains(cref_rect a, int x, int y) -> bool;
    auto contains(cref_rect a, cref_rect b) -> bool;
    auto intersects(cref_rect a, cref_rect b) -> bool;

    auto to_rectf(cref_rect a) -> rectf;
}
