/*  apps/quadwar/qw_factory.h
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef __quadwar__qw_factory__
#define __quadwar__qw_factory__

#include "../../laplace/engine/basic_factory.h"

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

#endif
