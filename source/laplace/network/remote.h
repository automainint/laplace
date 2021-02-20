/*  laplace/network/remote.h
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

#ifndef __laplace__network_remote__
#define __laplace__network_remote__

#include "udp_server.h"

namespace laplace::network {
  /*  Remote server.
   */
  class remote final : public udp_server {
  public:
    remote();
    ~remote() final;

    void connect(std::string_view host_address,
                 uint16_t         host_port,
                 uint16_t         client_port = any_port);

    void queue(cref_vbyte seq) final;

  private:
    void cleanup();

    auto perform_control(size_t slot, cref_vbyte seq)
        -> bool override;
  };
}

#endif
