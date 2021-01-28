#include "world.h"
#include "../world.h"

using namespace laplace;
using namespace engine;
using namespace object;

access::world::world(world &&w)
    :   m_world(w.m_world),
        m_mode(m_mode)
{ 
    w.m_mode = forbidden;
}

access::world::world(ref_world w, mode access_mode)
    :   m_world(w),
        m_mode(access_mode)
{ }

void access::world::desync()
{
    if (m_mode != forbidden)
    {
        m_world.desync();
    }
}

auto access::world::reserve(size_t id) -> size_t
{
    size_t result = id_undefined;

    if (m_mode == sync)
    {
        result = m_world.reserve(id);
    }

    return result;
}

auto access::world::spawn(ptr_entity ent, size_t id) -> size_t
{
    size_t result = id_undefined;

    if (m_mode == sync)
    {
        result = m_world.spawn(ent, id);
    }

    return result;
}

void access::world::remove(size_t id)
{
    if (m_mode == sync)
    {
        m_world.remove(id);
    }
}

void access::world::respawn(size_t id)
{
    if (m_mode == sync)
    {
        m_world.respawn(id);
    }
}

void access::world::clear()
{
    if (m_mode == sync)
    {
        m_world.clear();
    }
}

void access::world::queue(ptr_impact ev)
{
    if (m_mode != forbidden)
    {
        m_world.queue(ev);
    }
}

void access::world::set_root(ptr_entity root)
{
    if (m_mode == sync)
    {
        m_world.set_root(root);
    }
}

auto access::world::get_root() -> access::entity
{
    return { m_world.get_root(), m_mode };
}

auto access::world::has_entity(size_t id) -> bool
{
    return m_world.get_entity(id) ? true : false;
}

auto access::world::get_entity(size_t id) -> access::entity
{
    return { m_world.get_entity(id), m_mode };
}

auto access::world::select(condition op) -> access::ventity
{
    return { m_world.select(op), m_mode };
}

auto access::world::select_dynamic(condition op) -> access::ventity
{
    return { m_world.select_dynamic(op), m_mode };
}

auto access::world::get_random_engine() -> ref_rand
{
    return m_world.get_random();
}
