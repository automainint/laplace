#pragma once

#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <span>
#include <cstdint>

namespace laplace
{
    using ref_uint      = size_t &;
    using ref_char32    = char32_t &;

    using ref_string    = std::string &;
    using ref_wstring   = std::wstring &;

    using vbyte         = std::vector<uint8_t>;
    using vuint         = std::vector<size_t>;
    using vuint16       = std::vector<uint16_t>;
    using vuint32       = std::vector<uint32_t>;

    using ref_vbyte     = vbyte &;
    using ref_vuint     = vuint &;
    using ref_vuint16   = vuint16 &;
    using ref_vuint32   = vuint32 &;

    using cref_vbyte    = std::span<const uint8_t>;
    using cref_vuint    = std::span<const size_t>;
    using cref_vuint16  = std::span<const uint16_t>;
    using cref_vuint32  = std::span<const uint32_t>;
    using cref_vstring  = std::span<const std::string>;

    using ref_istream   = std::istream &;
    using ref_ostream   = std::ostream &;

    void log(std::string_view s);
    void log(const char *c_format, ...);
}
