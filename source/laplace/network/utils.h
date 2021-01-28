#pragma once

#include "defs.h"
#include "../core/defs.h"
#include "../platform/socket.h"

namespace laplace::network
{
    auto clamp_chunk(size_t size) -> int;
    auto to_string(const sockaddr &a) -> std::string;
    auto set_mode(SOCKET &s, io_mode m) -> bool;
}
