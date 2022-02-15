/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_NETWORK_SLOT_STATE_H
#define LAPLACE_NETWORK_SLOT_STATE_H

#include "defs.h"
#include "interface/io.h"
#include "transfer.h"

namespace laplace::network {
  struct event_queue {
    sl::index         index = 0;
    sl::vector<vbyte> events;
  };

  struct slot_state {
    std::string address;
    uint16_t    port = any_port;
    vbyte       token;

    sl::index id_actor      = id_undefined;
    bool      is_connected  = true;
    bool      is_exclusive  = false;
    bool      request_flag  = true;
    sl::time  last_received = 0;
    sl::time  last_sent     = 0;

    sl::vector<vbyte> in;
    sl::vector<vbyte> out;

    event_queue queue;
    transfer    tran;

    std::unique_ptr<io_interface::node> node;
  };
}

#endif
