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

#ifndef __laplace__engine_host__
#define __laplace__engine_host__

#include "udp_server.h"

namespace laplace::network {
  /*  Host server.
   */
  class host final : public udp_server {
  public:
    host();
    ~host() final = default;

    void listen(uint16_t port = any_port);

  private:
    auto perform_control(size_t slot, cref_vbyte seq) -> bool override;
  };
}

#endif
