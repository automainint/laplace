#ifndef __laplace__engine_protocol_slot_create__
#define __laplace__engine_protocol_slot_create__

#include "../basic_impact.h"

namespace laplace::engine::protocol {
  class slot_create : public sync_impact {
  public:
    enum encoding_offset : size_t {
      n_slot_actor = 18,
      n_local_flag = 26
    };

    static constexpr uint16_t id   = ids::slot_create;
    static constexpr size_t   size = 27;

    ~slot_create() override = default;

    constexpr slot_create() {
      set_size(size);
    }

    constexpr slot_create(bool is_local) {
      set_size(size);

      m_is_local = is_local;
    }

    constexpr slot_create(size_t id_host_actor) {
      set_actor(id_host_actor);
      set_size(size);

      m_is_local = true;
    }

    constexpr slot_create(size_t index, uint64_t time,
        size_t id_actor, bool is_local) {
      set_order({ index });
      set_time(time);
      set_actor(id_actor);
      set_size(size);

      m_is_local = is_local;
    }

    inline void perform(access::world w) const override {
      w.reserve(get_actor());
    }

    constexpr auto is_local() const {
      return m_is_local ? true : false;
    }

    static constexpr auto get_slot_actor(cref_vbyte seq) {
      return as_index(rd<uint64_t>(seq, n_slot_actor));
    }

    static constexpr auto get_local_flag(cref_vbyte seq) {
      return rd<uint8_t>(seq, n_local_flag);
    }

    inline void encode_to(
        std::span<uint8_t> bytes) const final {
      uint8_t flag_local = is_local() ? 1 : 0;

      write_bytes(bytes, id, get_index64(), get_time64(),
          get_actor64(), flag_local);
    }

    static constexpr auto scan(cref_vbyte seq) {
      return seq.size() == size && get_id(seq) == id;
    }

    static inline auto decode(cref_vbyte seq) {
      return slot_create { get_index(seq), get_time(seq),
        get_slot_actor(seq), get_local_flag(seq) > 0 };
    }

    /*  Alter the local status.
     *
     *  The actor is local if receiver slot
     *  actor is equal to the basic_impact actor.
     */
    static constexpr void alter(
        std::span<uint8_t> seq, size_t id_slot_actor) {
      wr<uint8_t>(seq, n_local_flag,
          (get_slot_actor(seq) == id_slot_actor));
    }

  private:
    bool m_is_local = false;
  };
}

#endif
