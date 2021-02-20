/*  laplace/engine/protocol/basic_event.h
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

#ifndef __laplace__engine_protocol_basic_event__
#define __laplace__engine_protocol_basic_event__

#include "../prime_impact.h"

namespace laplace::engine::protocol {
  class _non_inheritable { };
  using _inheritable = decltype([](access::world) {});

  template <typename perform_>
  concept perform_method = requires(access::world w) {
    perform_()(std::move(w));
  }
  || std::is_same<perform_, _non_inheritable>::value;

  template <                                      //
      uint16_t       id_,                         //
      perform_method perform_ = _non_inheritable, //
      typename base_class_    = prime_impact>

  class basic_event : public base_class_ {
  public:
    static constexpr uint16_t id   = id_;
    static constexpr size_t   size = 10;

    ~basic_event() override = default;

    constexpr basic_event() {
      this->set_encoded_size(size);
    }

    constexpr basic_event(size_t index) {
      this->set_order({ index });
      this->set_encoded_size(size);
    }

    inline void perform(access::world w) const override {
      perform_()(std::move(w));
    }

    inline void encode_to(std::span<uint8_t> bytes) const override {
      write_bytes(bytes, id, this->get_index64());
    }

    static constexpr auto scan(cref_vbyte seq) {
      return seq.size() == size && prime_impact::get_id(seq) == id;
    }

    static inline auto decode(cref_vbyte seq) {
      return basic_event { prime_impact::get_index(seq) };
    }
  };

  template <        //
      uint16_t id_, //
      typename base_class_>

  class basic_event<id_, _non_inheritable, base_class_> final
      : public base_class_ {
  public:
    static constexpr uint16_t id   = id_;
    static constexpr size_t   size = 10;

    ~basic_event() final = default;

    constexpr basic_event() {
      this->set_encoded_size(size);
    }

    constexpr basic_event(size_t index) {
      this->set_order({ index });
      this->set_encoded_size(size);
    }

    inline void encode_to(std::span<uint8_t> bytes) const final {
      write_bytes(bytes, id, this->get_index64());
    }

    static constexpr auto scan(cref_vbyte seq) {
      return seq.size() == size && prime_impact::get_id(seq) == id;
    }

    static inline auto decode(cref_vbyte seq) {
      return basic_event { prime_impact::get_index(seq) };
    }
  };

  template <uint16_t id_>
  using inheritable_event = basic_event<id_, _inheritable>;

  template <uint16_t id_>
  using inheritable_sync_event =
      basic_event<id_, _inheritable, sync_prime_impact>;

  /*  Events that can be handled: server_init, server_launch,
   *  client_ready, slot_create, slot_remove.
   */
  template <typename event_, typename perform_>
  using event_handler = basic_event<event_::id, perform_, event_>;

  using client_enter  = basic_event<ids::client_enter>;
  using client_leave  = basic_event<ids::client_leave>;
  using server_action = basic_event<ids::server_action>;
  using server_pause  = basic_event<ids::server_pause>;
  using server_quit   = basic_event<ids::server_quit>;

  using client_ready  = inheritable_event<ids::client_ready>;
  using server_init   = inheritable_sync_event<ids::server_init>;
  using server_launch = inheritable_sync_event<ids::server_launch>;
}

#endif
