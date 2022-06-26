/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "impact.h"

namespace laplace {
  std::pmr::synchronized_pool_resource impact_list::m_resource = {};

  impact_list::impact_list(impact i) noexcept :
      m_data({ i }, &m_resource) { }

  impact_list::impact_list(
      impact_list_intermediate const &list) noexcept :
      m_data(list.data.begin(), list.data.end(), &m_resource) { }
}
