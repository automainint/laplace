#include "qw_factory.h"
#include "qw_slot_create.h"

using namespace quadwar_app;
using namespace std;

auto qw_factory::decode(cref_vbyte seq) const
    -> engine::ptr_impact {
  if (qw_slot_create::scan(seq))
    return make<qw_slot_create>(seq);

  return decode_native(seq);
}
