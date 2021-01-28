#include "qw_slot_create.h"
#include "player.h"
#include "root.h"

using namespace quadwar_app;
using namespace engine;
using namespace std;

void qw_slot_create::perform(access::world w) const
{
    log(" :: event  Quadwar/slot_create: %s", is_local() ? "local" : "remote");

    w.spawn(make_shared<player>(is_local()), get_actor());
    root::slot_create(w.get_root(), get_actor());
}
