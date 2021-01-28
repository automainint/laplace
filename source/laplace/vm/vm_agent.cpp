#include "agent.h"

using namespace laplace;
using namespace vm;
using namespace std;

agent::agent() { }
agent::~agent() { }

void agent::set_environment(ptr_environment env)
{
    m_env = env;
}

auto agent::perform(size_t id, cref_vbyte args) -> vbyte
{
    return { };
}

auto agent::get_env() const -> ptr_environment
{
    return m_env.lock();
}
