#ifndef __laplace__engine_access_world_impl__
#define __laplace__engine_access_world_impl__

namespace laplace::engine {
  template <typename distribution>
  inline auto access::world::random(distribution &dist) ->
      typename distribution::result_type {
    auto result = distribution::result_type(0);

    if (this->m_mode == access::sync) {
      result = dist(get_random_engine());
    }

    return result;
  }
}

#endif
