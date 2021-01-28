#include "root.h"
#include "sets.h"
#include <laplace/core/utils.h>
#include <algorithm>

using namespace quadwar_app;
using namespace engine;
using namespace std;

auto root::changed() -> bool
{
    auto _ul = unique_lock(m_lock);

    if (m_is_changed)
    {
        m_is_changed = false;
        return true;
    }

    return false;
}

auto root::get_slot_count() -> size_t
{
    auto _sl = shared_lock(m_lock);
    return m_slots.size();
}

auto root::get_slot(size_t index) -> size_t
{
    auto _sl = shared_lock(m_lock);
    return index < m_slots.size() ? m_slots[index] : id_undefined;
}

void root::slot_create(access::entity en, size_t id_actor)
{
    en.modify(sets::slot_create, pack_to_array(id_actor));
}

void root::slot_remove(access::entity en, size_t id_actor)
{
    en.modify(sets::slot_remove, pack_to_array(id_actor));
}

auto root::get_slot_count(access::entity en) -> size_t
{
    const auto data = en.request(sets::slot_count);
    return rd<size_t>(data, 0);
}

auto root::get_slot(access::entity en, size_t index) -> size_t
{
    const auto result = en.request(sets::slot_get, pack_to_array(index));

    return rd<size_t>(result, 0);
}

auto root::do_request(size_t id, cref_vbyte args) const -> vbyte
{
    if (id == sets::slot_count)
    {
        return pack_to_bytes(m_slots.size());
    }
    else if (id == sets::slot_get)
    {
        const auto index = rd<size_t>(args, 0);

        if (index < m_slots.size())
        {
            return pack_to_bytes(m_slots[index]);
        }
    }

    return { };
}

void root::do_modify(size_t id, cref_vbyte args)
{
    const auto id_actor = rd<size_t>(args, 0);
    const auto it = lower_bound(m_slots.begin(), m_slots.end(), id_actor);

    if (id == sets::slot_create)
    {
        log("root: slot_create");

        m_slots.emplace(it, id_actor);
    }
    else if (id == sets::slot_remove)
    {
        log("root: slot_remove");

        if (it != m_slots.end() && *it == id_actor)
            m_slots.erase(it);
    }

    m_is_changed = true;
}
