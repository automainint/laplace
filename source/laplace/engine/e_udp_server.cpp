/*  laplace/engine/e_udp_server.cpp
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

#include "../core/utils.h"
#include "protocol/prime.h"
#include "udp_server.h"

namespace laplace::engine {
  using namespace network;
  using namespace std;

  void udp_server::set_chunk_size(size_t size,
                                  size_t overhead) {
    m_overhead = overhead;
    m_buffer.resize(size + overhead);
  }

  auto udp_server::get_chunk_size()->size_t {
    return m_buffer.size();
  }

  auto udp_server::adjust_chunk_size(cref_vbyte chunk)->size_t {
    size_t chunk_size = 0;

    while (chunk_size + 2 < chunk.size()) {
      size_t size = rd<uint16_t>(chunk, chunk_size);

      if (chunk_size + size + 2 + m_overhead > m_buffer.size())
        break;

      chunk_size += size + 2;
    }

    return chunk_size;
  }

  void udp_server::append_event(vbyte & chunk, cref_vbyte seq) {
    const auto size   = seq.size();
    const auto offset = chunk.size();

    if (size <= numeric_limits<uint16_t>::max()) {
      chunk.resize(offset + size + 2);
      write_bytes(std::span<uint8_t> { chunk.begin() + offset,
                                       chunk.end() },
                  static_cast<uint16_t>(size), seq);
    } else {
      error(__FUNCTION__, "Incorrect event size: %zd.", size);
    }
  }
}
