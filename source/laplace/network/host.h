/*  laplace/network/host.h
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

#ifndef laplace_engine_host_h
#define laplace_engine_host_h

#include "udp_server.h"
#include "../engine/solver.h"

namespace laplace::network {
  /*  Host server.
   */
  class host final : public udp_server {
  public:
    host();
    ~host() final = default;

    void set_seed(engine::seed_type seed);

    void listen(uint16_t port = any_port);

  private:
    [[nodiscard]] auto perform_control(size_t slot, cref_vbyte seq)
        -> bool override;

    engine::seed_type m_seed = engine::solver::generate_seed();
  };
}

#endif
