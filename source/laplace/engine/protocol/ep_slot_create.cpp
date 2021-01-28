#include "slot_create.h"

using namespace laplace;
using namespace engine;
using namespace protocol;

void slot_create::perform(access::world w) const
{
    w.reserve(get_actor());
}
