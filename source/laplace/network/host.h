/*  Copyright (c) 2022 Mitya Selivanov
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
#include <random>

namespace laplace::network {
  class host final : public udp_server {
  public:
    host();
    ~host() final = default;

    void listen(uint16_t port = any_port);

  private:
    [[nodiscard]] auto perform_control(sl::index slot, span_cbyte seq)
        -> event_status override;

    [[nodiscard]] auto generate_token() -> vbyte;

    std::mt19937_64 m_rand;
    uint16_t        m_seed;
  };
}

#endif
