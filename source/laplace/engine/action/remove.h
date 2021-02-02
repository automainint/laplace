#ifndef __laplace__engine_action_remove__
#define __laplace__engine_action_remove__

#include "../basic_impact.h"

namespace laplace::engine::action {
  class remove final : public sync_impact {
  public:
    remove(size_t id_entity) {
      m_id_entity = id_entity;
    }

    ~remove() final = default;

    void perform(access::world w) const final {
      w.remove(m_id_entity);
    }

  private:
    size_t m_id_entity = 0;
  };
}

#endif
