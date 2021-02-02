#pragma once

#include "../basic_impact.h"

namespace laplace::engine::protocol {
  class request_events final : public basic_impact {
  public:
    enum encoding_offset : size_t { n_events = 2 };

    static constexpr uint16_t id         = ids::request_events;
    static constexpr size_t   event_size = sizeof(uint64_t);

    /*  ( Max size value - Header size ) / Event index size
     */
    static constexpr auto max_event_count =
        (max_size - n_events) / event_size;

    ~request_events() final = default;

    inline request_events() {
      set_size(n_events);
    }

    inline request_events(cref_vuint events) {
      set_size(n_events + events.size() * event_size);

      m_events.assign(events.begin(), events.end());
    }

    static constexpr auto get_event_count(cref_vbyte seq) {
      return (seq.size() - n_events) / event_size;
    }

    static constexpr auto get_event(
        cref_vbyte seq, size_t index) {
      return as_index(
          rd<uint64_t>(seq, n_events + index * event_size));
    }

    inline void encode_to(
        std::span<uint8_t> bytes) const final {
      wr<uint16_t>(bytes, 0, id);

      for (size_t i = 0; i < m_events.size(); i++)
        wr<uint64_t>(bytes, 4 + i * event_size, m_events[i]);
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
