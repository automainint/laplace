#include "object_manager.h"
#include <algorithm>

using namespace laplace;
using namespace core;
using namespace std;

void object_manager::setup(fn_load loader, wstring_view extension)
{
    auto _ul = unique_lock(m_lock);
    m_loaders.emplace(pair { extension, loader });
}

void object_manager::setup(fn_save saver, wstring_view extension)
{
    auto _ul = unique_lock(m_lock);
    m_savers.emplace(pair { extension, saver });
}

void object_manager::add(ptr_pool obj_pool)
{
    if (obj_pool)
    {
        auto _ul = unique_lock(m_lock);

        if (m_is_default_pool)
        {
            m_pools.clear();
            m_is_default_pool = false;
        }

        if (find(m_pools.begin(), m_pools.end(), obj_pool) == m_pools.end())
        {
            m_pools.emplace_back(obj_pool);
        }
    }
}

void object_manager::erase(ptr_pool obj_pool)
{
    if (obj_pool)
    {
        auto _ul = unique_lock(m_lock);

        auto i = find(m_pools.begin(), m_pools.end(), obj_pool);

        if (i != m_pools.end())
        {
            m_pools.erase(i);
        }
    }
}

void object_manager::store_to(ptr_pool obj_pool)
{
    if (obj_pool)
    {
        auto _ul = unique_lock(m_lock);

        auto i = find(m_pools.begin(), m_pools.end(), obj_pool);

        if (i != m_pools.end())
        {
            m_pools.erase(i);
            m_pools.emplace_back(obj_pool);
        }
    }
}

void object_manager::store(wstring_view file_name, ptr_element obj)
{
    if (obj)
    {
        default_pool();

        auto _sl = shared_lock(m_lock);
        locked_store(file_name, obj);
    }
}

auto object_manager::preload(wstring_view file_name) -> bool
{
    bool result = false;

    default_pool();

    auto _sl = shared_lock(m_lock);

    for (auto &p : m_pools)
    {
        if (p->contains(file_name))
        {
            result = true;
            break;
        }
    }

    if (!result)
    {
        auto obj = load_internal(file_name);

        if (obj)
        {
            m_pools.back()->store(file_name, obj);
            result = true;
        }
    }

    return result;
}

auto object_manager::load_raw(wstring_view file_name) -> ptr_element
{
    ptr_element result;

    default_pool();

    auto _sl = shared_lock(m_lock);

    /*  Looking in reverse order.
     */
    for (size_t i = m_pools.size(); i > 0;)
    {
        auto obj = m_pools[--i]->load(file_name);

        if (obj)
        {
            result = obj;
            break;
        }
    }

    if (!result)
    {
        auto obj = load_internal(file_name);

        if (obj)
        {
            m_pools.back()->store(file_name, obj);
            result = obj;
        }
    }

    return result;
}

auto object_manager::save(wstring_view file_name, ptr_element obj) -> bool
{
    bool result = false;

    if (obj)
    {
        default_pool();

        auto _sl = shared_lock(m_lock);

        locked_store(file_name, obj);

        result = save_internal(file_name, obj);
    }

    return result;
}

auto object_manager::has_no_pool() -> bool
{
    auto _sl = shared_lock(m_lock);
    return m_pools.empty();
}

void object_manager::default_pool()
{
    if (!has_no_pool())
    {
        auto _ul = unique_lock(m_lock);
        m_pools.emplace_back(make_shared<pool>());
        m_is_default_pool = true;
    }
}

void object_manager::locked_store(wstring_view file_name, ptr_element obj)
{
    bool stored = false;

    for (auto &p : m_pools)
    {
        if (p->contains(file_name))
        {
            p->store(file_name, obj);
            stored = true;
            break;
        }
    }

    if (!stored)
    {
        m_pools.back()->store(file_name, obj);
    }
}

auto object_manager::load_internal(wstring_view file_name) -> ptr_element
{
    ptr_element result;
    auto ext = get_ext(file_name);

    for (auto &ld : m_loaders)
    {
        if (ld.first.empty() || ld.first == ext)
        {
            result = ld.second(file_name);
            
            if (result)
            {
                result->file_name = file_name;
                break;
            }
        }
    }

    return result;
}

auto object_manager::save_internal(wstring_view file_name, ptr_element obj) -> bool
{
    bool result = false;
    auto ext = get_ext(file_name);

    for (auto &sv : m_savers)
    {
        if (sv.first.empty() || sv.first == ext)
        {
            result = sv.second(file_name, obj);
            if (result) break;
        }
    }

    return result;
}

auto object_manager::get_ext(wstring_view file_name) -> wstring
{
    const auto n0 = file_name.find_last_of(L"\\/");
    const auto n = file_name.find_last_of('.');

    if (n != wstring::npos && (n0 == wstring::npos || n > n0))
    {
        const auto s = file_name.substr(n + 1);
        return wstring(s.begin(), s.end());
    }

    return L"";
}
