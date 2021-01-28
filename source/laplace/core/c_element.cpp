#include "element.h"

using namespace laplace;
using namespace core;
using namespace std;

element::element()
{
    m_type_id = type_id();
}

element::~element() { }

auto element::get_type_id() const -> size_t
{
    return m_type_id;
}

void element::set_type_id(size_t id)
{
    m_type_id = id;
}

auto element::type_id() -> size_t
{
    static const type type_void;
    return type_void.get_id();
}

atomic_size_t element::type::m_count = 0;

element::type::type()
{
    m_id = m_count++;
}

element::type::~type() { }

auto element::type::get_id() const -> size_t
{
    return m_id;
}
