#include "entity.h"

using namespace laplace;
using namespace engine;

access::entity::entity(entity &&e)
    :   m_entity(e.m_entity),
        m_mode(e.m_mode)
{
    e.m_entity.reset();
    e.m_mode = forbidden;
}

access::entity::entity(ptr_entity ent, mode access_mode)
    :   m_entity(ent),
        m_mode(ent ? access_mode : forbidden)
{ }

auto access::entity::exist() const -> bool
{
    return m_entity ? true : false;
}

void access::entity::set_dynamic(bool is_dynamic)
{
    if (m_mode != forbidden)
    {
        m_entity->set_dynamic(is_dynamic);
    }
}

void access::entity::set_markable(bool is_markable)
{
    if (m_mode != forbidden)
    {
        m_entity->set_markable(is_markable);
    }
}

void access::entity::set_selectable(bool is_selectable)
{
    if (m_mode != forbidden)
    {
        m_entity->set_selectable(is_selectable);
    }
}

void access::entity::set_vulnerable(bool is_vulnerable)
{
    if (m_mode != forbidden)
    {
        m_entity->set_vulnerable(is_vulnerable);
    }
}

void access::entity::set_tick_period(size_t tick_period)
{
    if (m_mode != forbidden)
    {
        m_entity->set_tick_period(tick_period);
    }
}

void access::entity::set_bounds(cref_box val)
{
    if (m_mode != forbidden)
    {
        m_entity->set_bounds(val);
    }
}

auto access::entity::index_of(size_t id) -> size_t
{
    return m_mode != forbidden ? m_entity->index_of(id) : id_undefined;
}

auto access::entity::get_count() -> size_t
{
    return m_mode != forbidden ? m_entity->get_count() : 0;
}

auto access::entity::id_of(size_t index) -> size_t
{
    return m_mode != forbidden ? m_entity->id_of(index) : id_undefined;
}

auto access::entity::scale_of(size_t index) -> size_t
{
    return m_mode != forbidden ? m_entity->scale_of(index) : 0;
}

auto access::entity::get(size_t index) -> int64_t
{
    return m_mode != forbidden ? m_entity->get(index) : 0;
}

void access::entity::set(size_t index, int64_t value)
{
    if (m_mode != forbidden)
    {
        m_entity->set(index, value);
    }
}

void access::entity::apply_delta(size_t index, int64_t delta)
{
    if (m_mode != forbidden)
    {
        m_entity->apply_delta(index, delta);
    }
}

auto access::entity::request(size_t id, cref_vbyte args) -> vbyte
{
    return m_mode != forbidden ? m_entity->request(id, args) : vbyte { };
}

void access::entity::modify(size_t id, cref_vbyte args)
{
    if (m_mode == sync)
    {
        m_entity->modify(id, args);
    }
}

auto access::entity::is_changed() -> bool
{
    if (m_mode == sync)
    {
        return m_entity->is_changed();
    }

    return false;
}

auto access::entity::is_dynamic() -> bool
{
    return m_mode != forbidden ? m_entity->is_dynamic() : false;
}

auto access::entity::is_markable() -> bool
{
    return m_mode != forbidden ? m_entity->is_markable() : false;
}

auto access::entity::is_selectable() -> bool
{
    return m_mode != forbidden ? m_entity->is_selectable() : false;
}

auto access::entity::is_vulnerable() -> bool
{
    return m_mode != forbidden ? m_entity->is_vulnerable() : false;
}

auto access::entity::get_tick_period() -> size_t
{
    return m_mode != forbidden ? m_entity->get_tick_period() : 0;
}

auto access::entity::get_bounds() -> box
{
    return m_mode != forbidden ? m_entity->get_bounds() : box { };
}

auto access::entity::get_id() -> size_t
{
    return m_mode != forbidden ? m_entity->get_id() : id_undefined;
}
