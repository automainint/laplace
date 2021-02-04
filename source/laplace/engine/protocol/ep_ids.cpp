/*  laplace/engine/protocol/ep_ids.cpp
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

#include "ids.h"

namespace laplace::engine::protocol::ids {
  const std::string table[] = { "", "publickey",
    "requestevents", "clientping", "serverlaunch",
    "serveraction", "serverpause", "serverclock", "serverseed",
    "serverquit", "cliententer", "clientleave", "clientready",
    "debug", "slotcreate", "slotremove" };
}