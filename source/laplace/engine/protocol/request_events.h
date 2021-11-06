/*  Copyright (c) 2021 Mitya Selivanov
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
    enum encoding_offset : sl::index { n_events = 2 };

    static constexpr uint16_t  id         = ids::request_events;
    static constexpr sl::whole event_size = sizeof(sl::index64);

    static constexpr sl::whole max_event_count = (max_size - n_events) /
                                                 event_size;

    ~request_events() final = default;

    inline request_events() {
      set_encoded_size(n_events);
    }

    inline request_events(std::span<const sl::index> events) {
      set_encoded_size(n_events + events.size() * event_size);

      m_events.assign(events.begin(), events.end());
    }

    static constexpr auto get_event_count(span_cbyte seq) {
      return (seq.size() - n_events) / event_size;
    }

    static constexpr auto get_event(span_cbyte seq, size_t index) {
      return as_index(
          serial::rd<sl::index64>(seq, n_events + index * event_size));
    }

    inline void encode_to(std::span<uint8_t> bytes) const final {
      auto events = sl::vector<sl::index64>(m_events.size());
      for (sl::index i = 0; i < events.size(); i++) {
        events[i] = m_events[i];
      }

      serial::write_bytes(bytes, id,
                          std::span<const sl::index64>(events));
    }

    static constexpr auto scan(span_cbyte seq) {
      return seq.size() >= n_events && get_id(seq) == id;
    }

    static inline auto decode(span_cbyte seq) {
      auto events = sl::vector<sl::index>(get_event_count(seq));

      for (sl::index i = 0; i < events.size(); i++) {
        events[i] = get_event(seq, i);
      }

      return request_events { events };
    }

  private:
    sl::vector<sl::index> m_events;
  };
}

#endif
