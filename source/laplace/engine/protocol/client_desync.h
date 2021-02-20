/*  laplace/engine/protocol/client_desync.h
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

#ifndef __laplace__engine_protocol_client_desync__
#define __laplace__engine_protocol_client_desync__

#include "../prime_impact.h"

namespace laplace::engine::protocol {
  class client_desync final : public prime_impact {
  public:
    static constexpr uint16_t id   = ids::client_desync;
    static constexpr size_t   size = 2;

    ~client_desync() final = default;

    constexpr client_desync() {
      set_encoded_size(size);
    }

    inline void encode_to(std::span<uint8_t> bytes) const final {
      write_bytes(bytes, id);
    }

    static constexpr auto scan(cref_vbyte seq) -> bool {
      return seq.size() == size && get_id(seq) == id;
    }

    static inline auto decode(cref_vbyte seq) {
      return client_desync {};
    }
  };
}

#endif
