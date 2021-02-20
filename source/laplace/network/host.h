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
    ~host() final;

    void listen(uint16_t port = any_port);

    void queue(cref_vbyte seq) final;

  private:
    /*  Returns false if the event should be
     *  added to the main queue.
     *
     *  Events from the main queue will be
     *  transferred to all the clients.
     */
    auto perform_control(size_t slot, cref_vbyte seq)
        -> bool override;
  };
}

#endif
