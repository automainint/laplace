#include "../core/utils.h"
#include "protocol/prime.h"
#include "udp_server.h"

using namespace laplace;
using namespace engine;
using namespace network;
using namespace std;

void udp_server::set_chunk_size(size_t size, size_t overhead) {
  m_overhead = overhead;
  m_buffer.resize(size + overhead);
}

auto udp_server::get_chunk_size() -> size_t {
  return m_buffer.size();
}

auto udp_server::adjust_chunk_size(cref_vbyte chunk) -> size_t {
  size_t chunk_size = 0;

  while (chunk_size + 2 < chunk.size()) {
    size_t size = rd<uint16_t>(chunk, chunk_size);

    if (chunk_size + size + 2 + m_overhead > m_buffer.size())
      break;

    chunk_size += size + 2;
  }

  return chunk_size;
}

void udp_server::append_event(vbyte &chunk, cref_vbyte seq) {
  const auto size   = seq.size();
  const auto offset = chunk.size();

  if (size <= numeric_limits<uint16_t>::max()) {
    chunk.resize(offset + size + 2);
    wr<uint16_t>(chunk, offset, static_cast<uint16_t>(size));
    copy(seq.begin(), seq.end(), chunk.begin() + offset + 2);
  } else {
    error(__FUNCTION__, "Incorrect event size: %zd.", size);
  }
}
