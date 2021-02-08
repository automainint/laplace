/*  laplace/vm/environment.predef.h
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

#ifndef __laplace__vm_environment_predef__
#define __laplace__vm_environment_predef__

#include <memory>

namespace laplace::vm {
  class environment;
  using ptr_environment = std::shared_ptr<environment>;
}

#endif
