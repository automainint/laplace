/*  laplace/engine/protocol/basic_value.h
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

#ifndef laplace_engine_protocol_basic_value_h
#define laplace_engine_protocol_basic_value_h

#include "../prime_impact.h"

namespace laplace::engine::protocol {
  template <uint16_t id_, typename type_>
  class basic_value final : public prime_impact {
  public:
    enum encoding_offset : sl::index { n_value = 10 };

    static constexpr uint16_t  id   = id_;
    static constexpr sl::whole size = n_value + sizeof(type_);

    ~basic_value() final = default;

    constexpr basic_value() {
      this->set_encoded_size(size);
    }

    constexpr basic_value(type_ value) {
      this->set_encoded_size(size);
      this->m_value = value;
    }

    constexpr basic_value(sl::index n, type_ value) {
      this->set_index(n);
      this->set_encoded_size(size);
      this->m_value = value;
    }

    static constexpr auto get_value(span_cbyte seq) {
      return serial::rd<type_>(seq, n_value);
    }

    inline void encode_to(std::span<uint8_t> bytes) const final {
      serial::write_bytes(bytes, id, this->get_index64(),
                          this->m_value);
    }

    static constexpr auto scan(span_cbyte seq) -> bool {
      return seq.size() == size && prime_impact::get_id(seq) == id;
    }

    static inline auto decode(span_cbyte seq) {
      return basic_value { prime_impact::get_index(seq),
                           get_value(seq) };
    }

  private:
    type_ m_value = 0;
  };

  template <uint16_t id_, typename type_>
  class instant_value final : public prime_impact {
  public:
    enum encoding_offset : sl::index { n_value = 2 };

    static constexpr uint16_t  id   = id_;
    static constexpr sl::whole size = n_value + sizeof(type_);

    ~instant_value() final = default;

    constexpr instant_value() {
      this->set_encoded_size(size);
    }

    constexpr instant_value(type_ value) {
      this->set_encoded_size(size);
      this->m_value = value;
    }

    static constexpr auto get_value(span_cbyte seq) {
      return serial::rd<type_>(seq, n_value);
    }

    inline void encode_to(std::span<uint8_t> bytes) const final {
      serial::write_bytes(bytes, id, this->m_value);
    }

    static constexpr auto scan(span_cbyte seq) -> bool {
      return seq.size() == size && prime_impact::get_id(seq) == id;
    }

    static inline auto decode(span_cbyte seq) {
      return instant_value { get_value(seq) };
    }

  private:
    type_ m_value = 0;
  };

  using ping_request  = instant_value<ids::ping_request, uint64_t>;
  using ping_response = instant_value<ids::ping_response, uint64_t>;

  using server_clock = basic_value<ids::server_clock, uint64_t>;
  using server_seed  = basic_value<ids::server_seed, uint64_t>;
}

#endif
