/*  apps/quadwar/landscape.h
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

#ifndef __quadwar__landscape__
#define __quadwar__landscape__

#include "../../laplace/engine/basic_entity.h"

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

#endif
