#include "slot_remove.h"

using namespace laplace;
using namespace engine;
using namespace protocol;

void slot_remove::perform(access::world w) const
{
    w.remove(get_actor());
}
