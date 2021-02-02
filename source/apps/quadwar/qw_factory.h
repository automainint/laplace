#pragma once

#include <laplace/engine/basic_factory.h>

namespace quadwar_app {
  using namespace laplace;

  class qw_factory : public engine::basic_factory {
  public:
    ~qw_factory() override = default;

    auto decode(cref_vbyte seq) const
        -> engine::ptr_impact override;

  private:
  };
}
