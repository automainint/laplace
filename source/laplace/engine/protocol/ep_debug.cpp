#include "debug.h"
#include "../object/sets.h"
#include "../../core/utils.h"

using namespace laplace;
using namespace engine;
using namespace object;
using namespace protocol;
using namespace std;

void debug::perform(access::world w) const
{
    auto root = w.get_root();

    if (root.exist())
    {
        root.set(root.index_of(sets::debug_value), m_value);
    }
    else
    {
        auto e = make_shared<debug_root>();

        e->set(e->index_of(sets::debug_value), m_value);

        w.set_root(e);
    }
}

debug::debug_root::debug_root()
{
    setup_sets({
        { sets::debug_value, 1, 0 }
    });
}
