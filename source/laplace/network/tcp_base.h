/*  laplace/network/tcp_base.h
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

#ifndef laplace_network_tcp_base_h
#define laplace_network_tcp_base_h

#include "../core/defs.h"
#include "../platform/socket.h"
#include "defs.h"

namespace laplace::network {
  class tcp_base {
  public:
    static constexpr size_t address_size = 64;

    tcp_base();
    virtual ~tcp_base();

    [[nodiscard]] auto receive_to(uint8_t *p, size_t count,
                                  io_mode mode) -> size_t;

    [[nodiscard]] auto receive(size_t count, io_mode mode) -> vbyte;
    [[nodiscard]] auto send(cref_vbyte seq) -> size_t;

    [[nodiscard]] auto is_done() const -> bool;

    void close();

  protected:
    void done();

    bool   m_is_done;
    SOCKET m_socket;
  };
}

#endif
