#include "embedded.h"
#include <shared_mutex>
#include <algorithm>

using namespace std;

namespace laplace::embedded
{
    shared_mutex lock;

    static bool is_ready = false;

    extern vector<wstring>          aliases;
    extern vector<vector<uint8_t>>  bytes;

    void init()
    {
        if (!is_ready)
        {
            auto size = aliases.size();

            vuint indices(size);

            for (size_t i = 0; i < size; i++)
                indices[i] = i;

            sort(indices.begin(), indices.end(), [](size_t i, size_t j) -> bool {
                return aliases[i] < aliases[j];
            });

            vector<wstring>         sorted_aliases(size);
            vector<vector<uint8_t>> sorted_bytes(size);

            if (bytes.size() != size)
                bytes.resize(size);

            for (size_t i = 0; i < size; i++)
            {
                auto n = indices[i];

                sorted_aliases[i]   = aliases[n];
                sorted_bytes[i]     = bytes[n];
            }

            aliases = sorted_aliases;
            bytes   = sorted_bytes;

            is_ready = true;
        }
    }

    auto scan(wstring_view file_name) -> bool
    {
        return !file_name.empty() && file_name[0] == L':';
    }

    auto exists(wstring_view file_name) -> bool
    {
        auto _sl = shared_lock(lock);

        init();

        auto i = lower_bound(aliases.begin(), aliases.end(), file_name);
        return i != aliases.end() && *i == file_name;
    }

    auto open(wstring_view file_name) -> cref_vbyte
    {
        auto _ul = unique_lock(lock);

        init();

        auto i = lower_bound(aliases.begin(), aliases.end(), file_name);

        if (i != aliases.end() && *i == file_name)
        {
            return bytes[i - aliases.begin()];
        }

        return cref_vbyte();
    }
}
