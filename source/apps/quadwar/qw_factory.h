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

#ifndef quadwar_qw_factory_h
#define quadwar_qw_factory_h

#include "../../laplace/engine/basic_factory.h"
#include "defs.h"

namespace quadwar_app {
  class qw_factory : public engine::basic_factory {
  public:
    ~qw_factory() override = default;

    auto decode(span_cbyte seq) const
        -> engine::ptr_prime_impact override;
  };
}

#endif
