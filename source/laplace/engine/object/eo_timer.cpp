#include "../action/remove.h"
#include "timer.h"

using namespace laplace;
using namespace engine;
using namespace object;
using namespace std;

timer timer::m_proto(timer::proto);

timer::timer(proto_tag) :
    basic_entity(true, false, false, false) { }

timer::timer(impact_gen gen, uint64_t period, size_t count) {
  *this = m_proto;

  init(n_tick_period, period);

  reset_clock();

  m_count = count;
  m_gen   = gen;
}

void timer::tick(access::world w) {
  if (m_gen) {
    w.queue(m_gen());
  }

  if (m_count > 0 && --m_count == 0) {
    self_destruct(move(w));
  }
}
