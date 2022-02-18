/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_NETWORK_PROTOCOL_HELPERS_TEST_H
#define LAPLACE_NETWORK_PROTOCOL_HELPERS_TEST_H

#include "../../../laplace/core/serial.h"
#include "../../../laplace/network/interface/io.h"
#include "../../../laplace/network/transfer.h"

namespace laplace::test {
  [[nodiscard]] inline auto _send(network::ptr_node &node,
                                  uint16_t           port,
                                  network::transfer &tran,
                                  auto... args) noexcept -> bool {
    auto packed = tran.encode(
        network::transfer::wrap(serial::pack_to_bytes(args...)));
    return node->send("", port, packed) == packed.size();
  }

  [[nodiscard]] inline auto _receive(network::ptr_node &node,
                                     network::transfer &tran) noexcept
      -> sl::vector<vbyte> {
    static uint8_t buf[1024] = {};
    return tran.decode({ buf, buf + node->receive(buf) });
  }
}

#endif
