#pragma once

#include "../basic_impact.h"

namespace laplace::engine::protocol {
  class server_seed final : public basic_impact {
  public:
    enum encoding_offset : size_t { n_seed = 18 };

    static constexpr uint16_t id   = ids::server_seed;
    static constexpr size_t   size = 22;

    ~server_seed() final = default;

    constexpr server_seed(uint32_t seed) {
      set_size(size);

      m_seed = seed;
    }

    constexpr server_seed(
        size_t index, uint64_t time, uint32_t seed) {
      set_order({ index });
      set_time(time);
      set_size(size);

      m_seed = seed;
    }

    static constexpr auto get_seed(cref_vbyte seq) {
      return rd<uint32_t>(seq, n_seed);
    }

    inline void encode_to(
        std::span<uint8_t> bytes) const final {
      write_bytes(
          bytes, id, get_index64(), get_time64(), m_seed);
    }

    static constexpr auto scan(cref_vbyte seq) {
      return seq.size() == size && get_id(seq) == id;
    }

    static inline auto decode(cref_vbyte seq) {
      return server_seed { get_index(seq), get_time(seq),
        get_seed(seq) };
    }

  private:
    uint32_t m_seed = 0;
  };
}
