#include "actor.h"
#include "sets.h"

using namespace laplace;
using namespace engine;
using namespace object;

size_t actor::n_is_local = 0;

actor::actor(dummy_tag) : entity(dummy) { }

actor::actor(proto_tag)
{
    setup_sets({
        { sets::is_local, 1, 0 }
    });

    n_is_local = index_of(sets::is_local);
}

void actor::set_local(access::entity en, bool is_local)
{
    en.set(n_is_local, is_local);
}

auto actor::is_local(access::entity en) -> bool
{
    return en.get(n_is_local) > 0 ? true : false;
}
