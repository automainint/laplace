#pragma once

#include "defs.h"

namespace laplace::format
{
    class buffer
    {
    public:
        buffer(fn_read read);
        ~buffer();

        auto read(size_t count) -> vbyte;

        void keep();
        void restore();

    private:
        fn_read m_read;
        size_t m_offset;
        vbyte m_data;
    };
}
