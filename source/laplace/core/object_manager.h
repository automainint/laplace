#pragma once

#include "pool.h"
#include <shared_mutex>
#include <functional>
#include <map>

namespace laplace::core
{
    using fn_load = std::function<ptr_element (std::wstring_view)>;
    using fn_save = std::function<bool (std::wstring_view, ptr_element)>;

    class object_manager
    {
    public:
        void setup(fn_load loader, std::wstring_view extension = L"");
        void setup(fn_save saver, std::wstring_view extension = L"");

        void add(ptr_pool obj_pool);
        void erase(ptr_pool obj_pool);
        void store_to(ptr_pool obj_pool);

        void store(std::wstring_view file_name, ptr_element obj);
        auto preload(std::wstring_view file_name) -> bool;
        auto load_raw(std::wstring_view file_name) -> ptr_element;
        auto save(std::wstring_view file_name, ptr_element obj) -> bool;

        template <typename type>
        auto load(std::wstring_view file_name) -> std::shared_ptr<type>;

    private:
        auto has_no_pool() -> bool;
        void default_pool();
        void locked_store(std::wstring_view file_name, ptr_element obj);
        auto load_internal(std::wstring_view file_name) -> ptr_element;
        auto save_internal(std::wstring_view file_name, ptr_element obj) -> bool;

        static auto get_ext(std::wstring_view file_name) -> std::wstring;

        std::shared_mutex               m_lock;
        bool                            m_is_default_pool = false;
        vptr_pool                       m_pools;
        std::map<std::wstring, fn_load> m_loaders;
        std::map<std::wstring, fn_save> m_savers;
    };
}

#include "object_manager.impl.h"
