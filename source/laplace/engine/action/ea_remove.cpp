#include "remove.h"
#include "../access/world.h"

using namespace laplace;
using namespace engine;
using namespace action;

remove::remove(size_t id_entity)
{
    m_id_entity = id_entity;
}

void remove::perform(access::world w) const
{
    w.remove(m_id_entity);
}
