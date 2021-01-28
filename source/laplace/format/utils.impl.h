#pragma once

namespace laplace::format
{
    inline auto wrap(ref_istream in) -> fn_read
    {
        return [&in](size_t n) -> vbyte {
            if (in)
            {
                vbyte v(n);
                auto p = in.tellg();
                in.read(reinterpret_cast<char *>(v.data()), n);
                v.resize(static_cast<size_t>(in.tellg() - p));
                return v;
            }

            return { };
        };
    }

    inline auto wrap(ref_ostream out) -> fn_write
    {
        return [&out](cref_vbyte data) -> size_t {
            if (out)
            {
                auto p = out.tellp();
                out.write(reinterpret_cast<const char *>(data.data()), data.size());
                return static_cast<size_t>(out.tellp() - p);
            }

            return 0;
        };
    }
}
