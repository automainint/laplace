#pragma once

#include "element.h"
#include <shared_mutex>

namespace laplace::core
{
    /*  Object pool of elements.
     */
    class pool
    {
    public:
        void clear();
        void store(std::wstring_view file_name, ptr_element elem);
        void release(std::wstring_view file_name);
        auto load(std::wstring_view file_name) -> ptr_element;
        auto contains(std::wstring_view file_name) -> bool;

    private:
        /*  Comparison operator for lower bound.
         */
        static auto op(const ptr_element &a, std::wstring_view b) -> bool;

        std::shared_mutex   m_lock;
        vptr_element        m_elems;
    };

    using cref_pool = const pool &;
    using ref_pool = pool &;
    using ptr_pool = std::shared_ptr<pool>;
    using vptr_pool = std::vector<ptr_pool>;
}
