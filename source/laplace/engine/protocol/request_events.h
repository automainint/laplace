/*  laplace/engine/protocol/request_events.h
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

#ifndef laplace_engine_protocol_request_events_h
#define laplace_engine_protocol_request_events_h

#include "../prime_impact.h"

namespace laplace::engine::protocol {
  class request_events final : public prime_impact {
  public:
    enum encoding_offset : size_t { n_events = 2 };

    static constexpr uint16_t id         = ids::request_events;
    static constexpr auto     event_size = sizeof(uint64_t);

    static constexpr auto max_event_count = (max_size - n_events) /
                                            event_size;

    ~request_events() final = default;

    inline request_events() {
      set_encoded_size(n_events);
    }

    inline request_events(cref_vuint events) {
      set_encoded_size(n_events + events.size() * event_size);

      m_events.assign(events.begin(), events.end());
    }

    static constexpr auto get_event_count(cref_vbyte seq) {
      return (seq.size() - n_events) / event_size;
    }

    static constexpr auto get_event(cref_vbyte seq, size_t index) {
      return as_index(
          rd<uint64_t>(seq, n_events + index * event_size));
    }

    inline void encode_to(std::span<uint8_t> bytes) const final {
      std::vector<uint64_t> events(m_events.size());
      for (size_t i = 0; i < events.size(); i++)
        events[i] = m_events[i];

      write_bytes(bytes, id, std::span<const uint64_t>(events));
    }

    static constexpr auto scan(cref_vbyte seq) {
      return seq.size() >= n_events && get_id(seq) == id;
    }

    static inline auto decode(cref_vbyte seq) {
      vuint events(get_event_count(seq));

      for (size_t i = 0; i < events.size(); i++)
        events[i] = get_event(seq, i);

      return request_events { events };
    }

  private:
    vuint m_events;
  };
}

#endif
