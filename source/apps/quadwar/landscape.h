#pragma once

#include <laplace/engine/basic_entity.h>

namespace quadwar_app {
  using namespace laplace;

  class landscape : public engine::basic_entity {
  public:
    landscape();

  private:
    landscape(int);

    static landscape m_proto;
  };
}
