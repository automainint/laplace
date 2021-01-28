#pragma once

#include <cstdint>

namespace laplace::graphics
{
    void rgb_to_hsl(const float *rgb, float *hsl);
    void hsl_to_rgb(const float *hsl, float *rgb);

    auto convert_color_channel(float x) -> uint8_t;
    auto convert_color_channel(uint8_t x) -> float;

    void convert_rgb(const float *f_rgb, uint8_t *u8_rgb);
    void convert_rgb(const uint8_t *u8_rgb, float *f_rgb);

    void convert_rgba(const float *f_rgba, uint8_t *u8_rgba);
    void convert_rgba(const uint8_t *u8_rgba, float *f_rgba);
}
