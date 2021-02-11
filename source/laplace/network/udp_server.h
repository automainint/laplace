/*  laplace/network/udp_server.h
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

#ifndef __laplace__network_udp_server__
#define __laplace__network_udp_server__

#include "udp_node.h"
#include "server.h"

namespace laplace::network {
  struct event_queue {
    size_t             index = 0;
    std::vector<vbyte> events;
  };

  /*  Base class for host and
   *  remote servers.
   */
  class udp_server : public server {
  public:
    static constexpr size_t default_chunk_size     = 4096;
    static constexpr size_t default_chunk_overhead = 1024;

    ~udp_server() override = default;

    void
    set_chunk_size(size_t size,
                   size_t overhead = default_chunk_overhead);

  protected:
    auto get_chunk_size() -> size_t;
    auto adjust_chunk_size(cref_vbyte chunk) -> size_t;
    void append_event(vbyte &chunk, cref_vbyte seq);

    size_t m_overhead = default_chunk_overhead;
    vbyte  m_buffer =
        vbyte(default_chunk_size + default_chunk_overhead);
  };
}

#endif
