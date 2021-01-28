#pragma once

#include "defs.h"
#include <sstream>

namespace laplace
{
    class bytestreambuf final : public std::stringbuf
    {
    public:
        bytestreambuf(cref_vbyte bytes);
        ~bytestreambuf() final = default;
    };

    /*  span<const uint8_t> wrapper for std::istream.
     */
    class ibytestream final : public std::istream
    {
    public:
        ibytestream(cref_vbyte bytes);
        ~ibytestream() final;
    };
}
