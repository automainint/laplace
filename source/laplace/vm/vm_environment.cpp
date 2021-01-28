#include "environment.h"
#include "agent.h"
#include <algorithm>

using namespace laplace;
using namespace vm;
using namespace std;

environment::environment() { }
environment::~environment() { }

auto environment::add(size_t unique_id, ptr_agent a) -> size_t
{
    size_t result = m_ids.size();

    m_ids.emplace_back(pair { unique_id, a });

    return result;
}

auto environment::get_id(size_t unique_id) -> size_t
{
    size_t id = 0;

    for (; id < m_ids.size(); id++)
    {
        if (m_ids[id].first == unique_id)
            break;
    }

    return id;
}

auto environment::perform(size_t id, cref_vbyte args) -> vbyte
{
    vbyte result;

    if (id < m_ids.size())
    {
        auto &a = m_ids[id];

        result = a.second->perform(a.first, args);
    }

    return result;
}
