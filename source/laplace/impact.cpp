/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "impact.h"

namespace laplace {
  std::pmr::synchronized_pool_resource impact_list::m_resource = {};
}
