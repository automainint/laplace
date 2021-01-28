/*  laplace/engine/impact.predef.h
 *
 *      Impact declarations.
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

#pragma once

#include <memory>
#include <vector>

namespace laplace::engine
{
    class impact;
    using ptr_impact = std::shared_ptr<impact>;
    using vptr_impact = std::vector<ptr_impact>;
}
